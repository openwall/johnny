/*
 * Copyright (c) 2011 Shinnok <admin at shinnok.com>.
 * Copyright (c) 2011, 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>.
 * See LICENSE for details.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "translator.h"
#include "filetablemodel.h"

#include <QToolButton>
#include <QStringListModel>
#include <QFileDialog>
#include <QFile>
#include <QByteArray>
#include <QTextStream>
#include <QMessageBox>
#include <QClipboard>
#include <QThread>
#include <QTextCursor>
#include <QStandardPaths>
#include <QInputDialog>

#define INTERVAL_PICK_CRACKED 15
#define PASSWORD_TAB 0
#define CONSOLE_LOG_SEPARATOR "-------------------------------------\n"

MainWindow::MainWindow(QSettings &settings)
    : QMainWindow(0),
      m_terminate(false),
      m_ui(new Ui::MainWindow),
      m_hashesTable(NULL),
      m_temp(NULL),
      m_settings(settings)
{
    // UI initializations
    m_ui->setupUi(this);
    // Until we get a result from john, we disable jumbo features
    m_isJumbo = false;
    setAvailabilityOfFeatures(false);
    connect(&m_johnVersionCheck, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(verifyJohnVersion()));

    // For the OS X QProgressBar issue
    // https://github.com/shinnok/johnny/issues/11
    m_ui->progressBar->installEventFilter(this);

    m_ui->listWidgetTabs->setAttribute(Qt::WA_MacShowFocusRect, false);

    // We select the PASSWORDS tab
    m_ui->contentStackedWidget->setCurrentIndex(PASSWORD_TAB);
    m_ui->listWidgetTabs->setCurrentRow(PASSWORD_TAB);
    // Disable copy button since there is no hash_tables (UI friendly)
    m_ui->actionCopyToClipboard->setEnabled(false);
    m_ui->actionStartAttack->setEnabled(false);
    foreach (QListWidgetItem *item, m_ui->listWidgetTabs->findItems("*", Qt::MatchWildcard))
        item->setSizeHint(QSize(m_ui->listWidgetTabs->width(), m_ui->listWidgetTabs->sizeHintForRow(0)));

    m_ui->attackModeTabWidget->setCurrentWidget(m_ui->defaultModeTab);
    m_ui->attackModeTabWidget->tabBar()->installEventFilter(this);

    // Multiple sessions management menu
    m_sessionMenu = new Menu(this);
    QToolButton *sessionMenuButton = new QToolButton(this);
    sessionMenuButton->setDefaultAction(m_ui->actionOpenLastSession);
    sessionMenuButton->setMenu(m_sessionMenu);
    sessionMenuButton->setPopupMode(QToolButton::InstantPopup);
    sessionMenuButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_ui->mainToolBar->insertWidget(m_ui->actionStartAttack, sessionMenuButton);
    m_ui->mainToolBar->insertSeparator(m_ui->actionStartAttack);
    connect(m_sessionMenu, SIGNAL(triggered(QAction*)), this, SLOT(actionOpenSessionTriggered(QAction*)));

    connect(&m_johnAttack, SIGNAL(finished(int, QProcess::ExitStatus)), this,
            SLOT(showJohnFinished(int, QProcess::ExitStatus)), Qt::QueuedConnection);
    connect(&m_johnAttack, SIGNAL(started()), this,
            SLOT(showJohnStarted()), Qt::QueuedConnection);
    connect(&m_johnAttack, SIGNAL(error(QProcess::ProcessError)), this,
            SLOT(showJohnError(QProcess::ProcessError)), Qt::QueuedConnection);
    connect(&m_johnAttack, SIGNAL(readyReadStandardOutput()), this,
            SLOT(updateJohnOutput()), Qt::QueuedConnection);
    connect(&m_johnAttack, SIGNAL(readyReadStandardError()), this,
            SLOT(updateJohnOutput()), Qt::QueuedConnection);

    // We connect timer with calling for john to show us status.
    connect(&m_showTimer, SIGNAL(timeout()),
            this, SLOT(callJohnShow()));
    // We connect 'john --show' process with our object.
    connect(&m_johnShow, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(readJohnShow()));

    connect(&m_hashTypeChecker,SIGNAL(updateHashTypes(const QStringList&, const QStringList& ,const QStringList&)), this,
            SLOT(updateHashTypes(const QStringList&,const QStringList&, const QStringList&)),Qt::QueuedConnection);
    connect(&m_passwordGuessing, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(callJohnShow()), Qt::QueuedConnection);
    connect(&m_passwordGuessing, SIGNAL(finished(int,QProcess::ExitStatus)), this,
            SLOT(guessPasswordFinished(int,QProcess::ExitStatus)), Qt::QueuedConnection);
    connect(&m_passwordGuessing, SIGNAL(error(QProcess::ProcessError)), this,
            SLOT(showJohnError(QProcess::ProcessError)), Qt::QueuedConnection);

    // Handling of buttons regarding settings
    connect(m_ui->pushButton_ResetSettings,SIGNAL(clicked()),
            this,SLOT(restoreSavedSettings()));
    connect(m_ui->pushButton_ApplySaveSettings,SIGNAL(clicked()),
            this,SLOT(applyAndSaveSettings()));
    // Settings changed by user
    connect(m_ui->spinBox_TimeIntervalPickCracked,SIGNAL(valueChanged(int)),this,SLOT(settingsChangedByUser()));
    connect(m_ui->comboBox_PathToJohn,SIGNAL(editTextChanged(QString)),this,SLOT(settingsChangedByUser()));
    connect(m_ui->comboBox_LanguageSelection,SIGNAL(currentIndexChanged(int)),this,SLOT(settingsChangedByUser()));
    connect(m_ui->checkBox_AutoApplySettings,SIGNAL(stateChanged(int)),this,SLOT(checkBoxAutoApplySettingsStateChanged()));

    // Action buttons
    connect(m_ui->actionOpenPassword,SIGNAL(triggered()),this,SLOT(openPasswordFile()));
    connect(m_ui->actionPauseAttack,SIGNAL(triggered()),this,SLOT(pauseAttack()));
    connect(m_ui->actionResumeAttack,SIGNAL(triggered()),this,SLOT(resumeAttack()));
    connect(m_ui->actionStartAttack,SIGNAL(triggered()),this,SLOT(startAttack()));
    connect(m_ui->pushButton_StatisticsUpdateStatus,SIGNAL(clicked()),this,SLOT(updateStatistics()));
    connect(m_ui->pushButton_WordlistFileBrowse,SIGNAL(clicked()),this,SLOT(buttonWordlistFileBrowseClicked()));
    connect(m_ui->pushButton_FillSettingsWithDefaults,SIGNAL(clicked()),this,SLOT(buttonFillSettingsWithDefaultsClicked()));
    connect(m_ui->pushButton_BrowsePathToJohn,SIGNAL(clicked()),this,SLOT(buttonBrowsePathToJohnClicked()));
    connect(m_ui->actionCopyToClipboard,SIGNAL(triggered()),this,SLOT(actionCopyToClipboardTriggered()));
    connect(m_ui->actionGuessPassword,SIGNAL(triggered()), this, SLOT(guessPassword()));

    connect(m_ui->listWidgetTabs,SIGNAL(itemSelectionChanged()),this,SLOT(listWidgetTabsSelectionChanged()));

    // We create the app data directory for us in $HOME if it does not exist.
    m_appDataPath = QDir::home().filePath(QLatin1String(".john") + QDir::separator() + "johnny" + QDir::separator());
    if (!QDir::home().mkpath(m_appDataPath)) {
        QMessageBox::critical( this, tr("Johnny"),
            tr("Could not create settings directory(%1). Check your permissions, disk space and restart Johnny.").arg(m_appDataPath));
        qApp->quit();
    }

    // Session for Johnny
    m_settings.beginGroup("johnSessions");
    QStringList sessionsList = m_settings.childGroups();
    for (int i = sessionsList.size()-1; i >= 0; i--) {
        QString sessionName = sessionsList[i];
        QString completePath = QDir(m_appDataPath).filePath(sessionName);
        if (QFileInfo(completePath + ".rec").isReadable()) {
            QAction* fileAction = m_sessionMenu->addAction(sessionName);
            fileAction->setData(sessionName);
            m_sessionHistory.append(sessionName);
            QString fileNames = m_settings.value(sessionName + "/passwordFiles").toStringList().join(" ");
            fileAction->setToolTip(fileNames);
        } else {
            // The .rec may have been deleted manually by user, let's clean our settings
            m_settings.remove(sessionName);
        }
    }
    m_settings.endGroup();
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
    m_sessionMenu->setToolTipsVisible(true);
#endif
    m_sessionMenu->addAction(m_ui->actionClearSessionHistory);
    
    // Automatically open last session by default
    if (!m_sessionHistory.isEmpty()) {
        m_session = QDir(m_appDataPath).filePath(m_sessionHistory[0]);
        openLastSession();
    } else {
        m_session.clear(); // No session
        restoreDefaultAttackOptions(false);
    }

    // We fill form with default values. Then we load settings. When
    // there is no setting old value is used. So if there is no
    // configuration file then we get default values. Also it means
    // that when user pushes "reset to stored settings" button he
    // resets only fields with values. It is nice but may seem
    // strange: probably user would expect reset to be like default +
    // reset because storing of a part of settings is not normal
    // behaviour (only possible with upgrades).
    fillSettingsWithDefaults();

    // We load old settings.
    restoreSavedSettings();

    Translator& translator = Translator::getInstance();
    m_ui->comboBox_LanguageSelection->insertItems(0, translator.getListOfAvailableLanguages());
    m_ui->comboBox_LanguageSelection->setCurrentText(translator.getCurrentLanguage());

    #if !OS_FORK
    //As of now, fork is only supported on unix platforms
        m_ui->widgetFork->hide();
    #endif

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!m_terminate && (m_johnAttack.state() != QProcess::NotRunning)) {
        int answer = QMessageBox::question(
            this,
            tr("Johnny"),
            tr("An attack session is running, it will be terminated if you proceed. Do you really want to quit?"),
            QMessageBox::Yes | QMessageBox::No);
        if (answer == QMessageBox::No) {
            event->ignore();
            return;
        }
    }
    m_terminate = true;
    event->accept();
}

MainWindow::~MainWindow()
{
    m_johnAttack.stop();
    m_johnShow.stop();
    m_johnVersionCheck.stop();
    m_hashTypeChecker.stop();
    m_passwordGuessing.stop();

    delete m_ui;
    m_ui = 0;
    delete m_hashesTable;
    m_hashesTable = 0;
    delete m_temp;
    m_temp = 0;
}

void MainWindow::buttonWordlistFileBrowseClicked()
{
    // We pop a dialog to choose a file to open.
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("World list files (*.lst) ;; Dict files (*.dict);; Any files (*)"));

    if (dialog.exec()) {
        QString fileName = dialog.selectedFiles()[0];
        // We put file name into field for it.
        m_ui->comboBox_WordlistFile->setEditText(fileName);
    }
}

void MainWindow::listWidgetTabsSelectionChanged()
{
    m_ui->contentStackedWidget->setCurrentIndex(m_ui->listWidgetTabs->currentRow());
    m_ui->actionCopyToClipboard->setEnabled(m_ui->listWidgetTabs->currentRow() == PASSWORD_TAB);
}

void MainWindow::replaceTableModel(QAbstractTableModel *newTableModel)
{
    // Remove temporary file is exist
    if (m_temp != NULL) {
        delete m_temp;
        m_temp = NULL;
    }

    // We delete existing model if any.
    if (m_hashesTable != NULL) {
        delete m_hashesTable;
        m_hashesTable = NULL;
    }
    // We remember new model.
    m_hashesTable = newTableModel;
    // We connect table view with new model.
    m_ui->tableView_Hashes->setModel(newTableModel);
    // Hide formats column if not jumbo
    m_ui->tableView_Hashes->setColumnHidden(FileTableModel::FORMATS_COL, !m_isJumbo);

    // We build hash table for fast access.
    m_showTableMap = QMultiMap<QString, int>();

    // In case a newTableModel == NULL parameter is passed
    if(m_hashesTable != NULL){
        for (int i = 0; i < m_hashesTable->rowCount(); i++) {
            m_showTableMap.insert(
                m_hashesTable->data(m_hashesTable->index(i, 2)).toString(),
                i);
        }
    }
}

bool MainWindow::readPasswdFiles(const QStringList &fileNames)
{
    FileTableModel *model = new FileTableModel(this);
    if (model->readFile(fileNames)) {
        // We replace existing model with new one.
        replaceTableModel(model);
        // After new model remembered we remember its file name.
        m_passwordFiles = fileNames;
        // We make a file with original hash in gecos to connect password
        // with original hash during `john --show`.
        if (!m_temp) {
            m_temp = new QTemporaryFile();
            if (m_temp->open()) {
                QTextStream temp(m_temp);
                for (int i = 0; i < m_hashesTable->rowCount(); i++) {
                    QString user = m_hashesTable->data(m_hashesTable->index(i, 0)).toString();
                    QString hash = m_hashesTable->data(m_hashesTable->index(i, 2)).toString();
                    temp << user << ":" << hash << "::" << hash << '\n';
                }
                m_temp->close();
            } else {
                QMessageBox::critical(
                    this,
                    tr("Johnny"),
                    tr("Can't open a temporary file. Your disk might be full."));
            }
        }
        callJohnShow();
        m_ui->actionCopyToClipboard->setEnabled(m_ui->contentStackedWidget->currentIndex() == PASSWORD_TAB);
        m_ui->actionStartAttack->setEnabled(true);
        m_ui->actionGuessPassword->setEnabled(true);
        if (m_isJumbo) {
            m_hashTypeChecker.setJohnProgram(m_pathToJohn);
            m_hashTypeChecker.setPasswordFiles(fileNames);
            m_hashTypeChecker.start();
        }
        return true;
    }
    QMessageBox::warning(
            this,
            tr("Johnny"),
            tr("Could not read desired password file(s)."));
    return false;
}

void MainWindow::openPasswordFile()
{
    // When user asks to open password file we should read desired
    // file, parse it and present values in the table. Model and view
    // simplifies presentation. We just make and fill model and then
    // we set it to existing view.

    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFiles);
    if (dialog.exec()) {
        QStringList fileNames = dialog.selectedFiles();
        readPasswdFiles(fileNames);
        m_ui->actionResumeAttack->setEnabled(false);
    }
}

void MainWindow::openLastSession()
{
    QString sessionName(m_session);
    sessionName.remove(m_appDataPath);
    m_settings.beginGroup("johnSessions/" + sessionName);
    QString format = m_settings.value("formatJohn").toString();
    QStringList fileNames = m_settings.value("passwordFiles").toStringList();
    m_settings.endGroup();

    if (readPasswdFiles(fileNames)) {
        m_format = format;
        restoreSessionOptions();
        m_ui->actionResumeAttack->setEnabled(true);
    }
}

void MainWindow::actionCopyToClipboardTriggered()
{
    if (!m_hashesTable)
        return;
    QModelIndexList indexes = m_ui->tableView_Hashes->selectionModel()->selectedIndexes();
    if (indexes.count() == 0)
    {
        QMessageBox::warning(
            this,
            tr("Johnny"),
            tr("Nothing selected. Select rows/columns in the Passwords table to copy."));
        return;
    }

    QString out;
    if (indexes.count() == 1) {
        out = indexes.at(0).data().toString();
    } else {
        qSort(indexes);
        int previousRow = -1;
        // TODO: such table making works bad with ctrl+mouse
        //       selection. I'd say in such case not selected fields
        //       inside the rectangle should be exported as empty.
        foreach (const QModelIndex &index, indexes) {
            // TODO: do it faster.
            // TODO: check for tabs inside.
            // TODO: use mimetype to make good tables.
            if (previousRow == index.row()) {
                out += "\t";
            } else if (previousRow != -1) {
                out += "\n";
            }
            out += index.data().toString();
            previousRow = index.row();
        }
    }
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->clear();
    if (clipboard->supportsSelection())
        clipboard->setText(out, QClipboard::Selection);
    clipboard->setText(out);
}

bool MainWindow::checkSettings()
{
    if (m_pathToJohn.isEmpty()) {
        QMessageBox::critical(
            this,
            tr("Johnny"),
            tr("Please specify the path to JohntheRipper in settings."));
        return false;
    }
    return true;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (!event)
        return false;
    if (!watched || !watched->isWidgetType())
        return false;
    QWidget* widget = (QWidget*) watched;
    switch (event->type())
    {
    case QEvent::StyleAnimationUpdate:
        if (widget->inherits("QProgressBar"))
            return true;
        break;
    case QEvent::Wheel:
        if (widget->inherits("QTabBar")) {
            event->ignore();
            return true;
        }
    default:
        break;
    }
    return false;
}

void MainWindow::startAttack()
{
    if (!checkSettings())
        return;
    
    // Session for johnny
    QString date = QDateTime::currentDateTime().toString("MM-dd-yy_hh:mm:ss");
    m_session = QDir(m_appDataPath).filePath(date);
    QString nameOfFile = m_session + ".rec";

    if (QFileInfo(nameOfFile).isReadable()) {
        int button = QMessageBox::question(
            this,
            tr("Johnny"),
            tr("Johnny is about to overwrite your previous session file. Do you want to proceed?"),
            QMessageBox::Yes | QMessageBox::No);
       if (button == QMessageBox::No)
            return;
        // Remove .rec file to avoid problem when john does not write it.
        if(!QFile(nameOfFile).remove())
        {
            QMessageBox::warning(
                this,
                tr("Warning"),
                tr("Unable to remove file ") + nameOfFile);
        }
    }

    QStringList parameters = saveAttackParameters();
    parameters << QString("--session=%1").arg(m_session);

    // We check that we have file name.
    if (!m_passwordFiles.isEmpty()) {
        // If file name is not empty then we have file, pass it to John.
        parameters << m_passwordFiles;
        startJohn(parameters);
    } else {
        QMessageBox::warning(
            this,
            tr("Johnny"),
            tr("No password files specified."));
    }
}

/* This function doesn't return the parameters of the current john process,
 * it returns the selected parameters in Johnny (UI-side) */
QStringList MainWindow::saveAttackParameters()
{
    QString sessionName(m_session);
    sessionName.remove(m_appDataPath);
    m_settings.beginGroup("johnSessions/" + sessionName);
    m_settings.setValue("passwordFiles", m_passwordFiles);
    
    QStringList parameters;
    // We prepare parameters list from options section.
    // General options
    // Format
    if (m_ui->comboBox_Format->currentText() != tr("Auto detect")) {
        // We have one list for formats and subformats. Subformats
        // contain short description after it.
        // Strings could be like:
        // des
        // md5_gen(0): md5($p)  (raw-md5)
        // md5_gen(1001) md5(md5(md5(md5($p))))
        // User formats does not have semicolon between format name
        // and description. So we will restrict our pattern with right
        // brace.
        // NOTE: $ john -format:md5-gen -subformat:md5_gen(0)
        //       is deprecated. So we will use
        //       $ john -format:md5_gen(0)
        //       instead. See
        //       http://www.openwall.com/lists/john-users/2011/08/17/2
        // We remember format key to be used with '-show' to take
        // progress.
        m_format = "--format=" + m_ui->comboBox_Format->currentText();
        // Now we have '--format=format' or '--format=format(N)description'.
        // So we truncate string to ')' if brace is in string.
        //
        // We try to find ')'.
        int index = m_format.indexOf(")");
        // We check that we have brace in string.
        if (index >= 0) {
            // We truncate line to index keeping index.
            m_format.truncate(index + 1);
        }
        // We add format key onto parameters list.
        parameters << m_format;
    } else {
        // In case we do not have explicit format we erase remembered
        // key.
        m_format.clear();
    }
    m_settings.setValue("formatJohn", m_format);
    m_settings.setValue("formatUI", m_ui->comboBox_Format->currentText());
    
    // Modes
    QWidget* selectedMode = m_ui->attackModeTabWidget->currentWidget();
    if (selectedMode == m_ui->defaultModeTab) {
        // Default behaviour - no modes
        // There are no options here.
        m_settings.setValue("mode", "default");
    } else if (selectedMode == m_ui->singleModeTab) {
        // "Single crack" mode
        parameters << "--single";
        m_settings.setValue("mode", "single");
        // External mode, filter
        if (m_ui->checkBox_SingleCrackModeExternalName->isChecked()) {
            parameters << ("--external=" + m_ui->comboBox_SingleCrackModeExternalName->currentText());
            m_settings.setValue("singleCrackExternalName", m_ui->comboBox_SingleCrackModeExternalName->currentText());
        }
        
    } else if (selectedMode == m_ui->wordlistModeTab) {
        // Wordlist mode
        m_settings.setValue("mode", "wordlist");
        parameters << ("--wordlist=" + m_ui->comboBox_WordlistFile->currentText());
        m_settings.setValue("wordlistFile", m_ui->comboBox_WordlistFile->currentText());
        
        // Rules
        if (m_ui->checkBox_WordlistModeRules->isChecked()) {
                parameters << "--rules";
        }
        m_settings.setValue("isUsingWordListRules", m_ui->checkBox_WordlistModeRules->isChecked());
        // External mode, filter
        if (m_ui->checkBox_WordlistModeExternalName->isChecked()) {
            parameters << ("--external=" + m_ui->comboBox_WordlistModeExternalName->currentText());
            m_settings.setValue("worldListExternalName", m_ui->comboBox_WordlistModeExternalName->currentText());
        }
    } else if (selectedMode == m_ui->incrementalModeTab) {
        // "Incremental" mode
        // It could be with or without name.
        m_settings.setValue("mode", "incremental");
        if (m_ui->checkBox_IncrementalModeName->isChecked()) {
            // With name
            parameters << ("--incremental=" + m_ui->comboBox_IncrementalModeName->currentText());
            m_settings.setValue("incrementalModeName", m_ui->comboBox_IncrementalModeName->currentText());
        } else {
            // Without name
            parameters << "--incremental";
        }
        // External mode, filter
        if (m_ui->checkBox_IncrementalModeExternalName->isChecked()) {
            parameters << ("--external=" + m_ui->comboBox_IncrementalModeExternalName->currentText());
            m_settings.setValue("incrementalExternalName", m_ui->comboBox_IncrementalModeExternalName->currentText());
        }
    } else if (selectedMode == m_ui->externalModeTab) {
        // External mode
        m_settings.setValue("mode", "external");       
        parameters << ("--external=" + m_ui->comboBox_ExternalModeName->currentText());
        m_settings.setValue("externalModeName", m_ui->comboBox_ExternalModeName->currentText());
    }

    // Selectors
    if (m_ui->checkBox_LimitUsers->isChecked()) {
        parameters << ("--users=" + m_ui->comboBox_LimitUsers->currentText());
        m_settings.setValue("limitUsers", m_ui->comboBox_LimitUsers->currentText());
    }
    if (m_ui->checkBox_LimitGroups->isChecked()) {
        parameters << ("--groups=" + m_ui->comboBox_LimitGroups->currentText());
        m_settings.setValue("limitGroups", m_ui->comboBox_LimitGroups->currentText());
    }
    if (m_ui->checkBox_LimitShells->isChecked()) {
        parameters << ("--shells=" + m_ui->comboBox_LimitShells->currentText());
        m_settings.setValue("limitShells", m_ui->comboBox_LimitShells->currentText());
    }
    if (m_ui->checkBox_LimitSalts->isChecked()) {
        parameters << (QString("--salts=%1").arg(m_ui->spinBox_LimitSalts->value()));
        m_settings.setValue("limitSalts", m_ui->spinBox_LimitSalts->value());
    }

    // Advanced options
    if (m_ui->checkBox_UseFork->isChecked()) {
        parameters << (QString("--fork=%1").arg(m_ui->spinBox_nbOfProcess->value()));
        m_settings.setValue("nbForkProcess", m_ui->spinBox_nbOfProcess->value());
    }
    m_settings.setValue("OMP_NUM_THREADS", m_ui->spinBox_nbOfOpenMPThread->value());
    if (m_ui->checkBox_EnvironmentVar->isChecked()) {
        m_settings.setValue("environmentVariables", m_ui->lineEdit_EnvironmentVar->text());
    }
    m_settings.endGroup();
    
    return parameters;
}

void MainWindow::startJohn(QStringList args)
{
    // To start John we have predefined process object. That object's
    // signals are already connected with our slots. So we need only
    // start it.

    // to visually separate sessions in the console output (make it clearer for the user)
    QString cmd = CONSOLE_LOG_SEPARATOR +
            QTime::currentTime().toString("hh:mm:ss : ") + m_pathToJohn + " " + args.join(" ") + '\n';

    appendLog(cmd);

    //We set up environment variables, ex : useful for openMP
    QProcessEnvironment env;
    // If default is chosen, we don't specify OMP_NUM_THREADS and john will choose the number of
    // threads based on the number of processors.
    if (m_ui->spinBox_nbOfOpenMPThread->text() != m_ui->spinBox_nbOfOpenMPThread->specialValueText()) {
        env.insert("OMP_NUM_THREADS", m_ui->spinBox_nbOfOpenMPThread->text()); // Add an environment variable
    }

    // User specified environment variables
    if (m_ui->checkBox_EnvironmentVar->isChecked()) {
        // Parse the input
        QStringList varList = m_ui->lineEdit_EnvironmentVar->text().split(",", QString::SkipEmptyParts);
        for (int i = 0; i < varList.size(); i++) {
            QStringList varPair = varList[i].split("=", QString::SkipEmptyParts);
            if (varPair.size() == 2) { // we assume value of variable doesn't have = inside
                env.insert(varPair[0].trimmed(), varPair[1].trimmed());
            } else {
                QMessageBox::warning(
                        this,
                        tr("Environment variables"),
                        tr("The format to set environment variable must be in the format : varName1="
                           "value, varName2=value etc.. "));
            }
        }
    }
    m_johnAttack.setEnv(env);
    m_johnAttack.setArgs(args);
    m_johnAttack.setJohnProgram(m_pathToJohn);
    m_johnAttack.start();

    // We remember date and time of the start.
    m_startDateTime = QDateTime::currentDateTime();
}


void MainWindow::resumeAttack()
{
    if (!checkSettings())
        return;

    QStringList parameters;
    parameters << QString("--restore=%1").arg(m_session);

    startJohn(parameters);
}

void MainWindow::updateJohnOutput()
{
    //read output and error buffers
    appendLog(m_johnAttack.readAllStandardOutput()
              + m_johnAttack.readAllStandardError());

    // NOTE: Probably here we want to parse John's output, catch newly
    //       cracked passwords and so on. However John's output is buffered.
    //       So we do not obtain it as soon as it occurs. Timer and
    //       --show work for us.
}

void MainWindow::pauseAttack()
{
    m_johnAttack.stop();
}

void MainWindow::showJohnStarted()
{
    // We disable all buttons.
    m_ui->actionPauseAttack->setEnabled(false);
    m_ui->actionStartAttack->setEnabled(false);
    m_ui->actionResumeAttack->setEnabled(false);
    m_ui->actionOpenPassword->setEnabled(false);
    m_ui->actionOpenLastSession->setEnabled(false);

    // When John starts we enable stop button.
    m_ui->actionPauseAttack->setEnabled(true);
    // When John starts we start capturing passwords.
    // TODO: When user change respective setting time for current run
    //       is not changed. Probably user expects other.
    // TODO: Should we distinguish settings for current run and for
    //       next runs? At least we should mark settings that could be
    //       changed while John works as having such behaviour.
    // NOTE: m_timeIntervalPickCracked is limited to int (maximum is
    //       MAX_INT) but here we multiple it by 1000. So we could
    //       have integer overflow. At this time value is borrowed from
    //       spin box. It could have customized limit. But currently
    //       we do not want to set it up at runtime so we should use
    //       value appropriate for multiple platforms but enough big
    //       to be comfortable for users.
    //       So current value corresponds to one day. However it is
    //       not suitable for platforms with int of two bytes.
    m_showTimer.start(m_timeIntervalPickCracked * 1000);
    // If we continue cracking than there could already be cracked
    // passwords so we check status.
    callJohnShow();
}

void MainWindow::showJohnError(QProcess::ProcessError error)
{
    QString message;
    switch (error) {
    case QProcess::FailedToStart:
        message = tr("John failed to start."
                     "Check your path to John."
                     "Check permissions on the executable.");
        break;

    case QProcess::Crashed:
        message = tr("John crashed. Verify Console Log for details.");
        break;

    case QProcess::Timedout:
        message = tr("A timed out error happened to John.");
        break;

    case QProcess::WriteError:
        message = tr("A write error happened to John.");
        break;

    case QProcess::ReadError:
        message = tr("A read error happened to John.");
        break;

    case QProcess::UnknownError:
        message = tr("An unknown problem happened to John. Verify Console Log for any details.");
        break;
    }

    QMessageBox::critical(this, tr("Johnny"), message + "(" + m_pathToJohn + ")");

    if (QObject::sender() == &m_passwordGuessing) {
        m_ui->actionGuessPassword->setEnabled(true);
    }

}

void MainWindow::showJohnFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    appendLog(CONSOLE_LOG_SEPARATOR);

    QString sessionName(m_session);
    sessionName.remove(m_appDataPath);
   
    bool isNewSession = !m_sessionHistory.contains(sessionName);
    bool isRecReadable = QFileInfo(m_session + ".rec").isReadable();
    if ((isNewSession == true) && (isRecReadable == true)) {
        // New session saved by john, add it to the list
        QAction* action = new QAction(sessionName,this);
        m_sessionMenu->insertAction(m_sessionMenu->actions()[0],action);
        action->setData(sessionName);
        m_sessionHistory.append(sessionName);
        action->setToolTip(m_passwordFiles.join(" "));
    } else if ((isNewSession == false) && (isRecReadable == false)) {
        // An old session (which was resumed) terminated and it can no longer be resumed (john deleted .rec)
        // so we remove it from the session history list to have an error-prone UI
        m_sessionHistory.removeOne(sessionName);
        m_settings.remove("johnSessions/" + sessionName);
        foreach(QAction* actions, m_sessionMenu->actions()) {
            if (actions->data().toString() == sessionName) {
                    m_sessionMenu->removeAction(actions);
            }
        }
    }

    // When John finishes we enable start button and disable stop
    // button.
    m_ui->actionPauseAttack->setEnabled(false);
    m_ui->actionStartAttack->setEnabled(true);
    m_ui->actionOpenPassword->setEnabled(true);
    m_ui->actionOpenLastSession->setEnabled(true);
    m_ui->actionResumeAttack->setEnabled(m_sessionHistory.contains(sessionName) && isRecReadable);

    if (exitStatus == QProcess::CrashExit) {
        qDebug() << "JtR seems to have crashed.";
        return;
    }
    // When John stops we need to stop timer and to look status last
    // time.
    m_showTimer.stop();
    callJohnShow();
}

void MainWindow::callJohnShow()
{
    // Give a chance to terminate cleanly
    if (m_johnShow.state() != QProcess::NotRunning)
        m_johnShow.stop();

    QStringList args;
    // We add current format key if it is not empty.
    if (!m_format.isEmpty())
        args << m_format;
    args << "--show" << m_temp->fileName();
    m_johnShow.setJohnProgram(m_pathToJohn);
    m_johnShow.setArgs(args);
    m_johnShow.start();
}

void MainWindow::readJohnShow()
{
    // We read all output.
    QByteArray output = m_johnShow.readAllStandardOutput();
    QTextStream outputStream(output);
    // We parse it.
    // We read output line by line and take user name and password.
    // Then we find a row with such user and insert password there.
    QString line;
    line = outputStream.readLine();
    // If john did not yet cracked anything then john does not emit
    // empty line before statistics.
    QString firstLine;
    firstLine = line;
    // We read to the end or before empty line.
    while (!line.isNull() && line != "") {
        line.remove(QRegExp("\\r?\\n"));
        // We split lines to fields.
        int left = line.indexOf(":");
        int right = line.lastIndexOf("::");
        QString password = line.mid(left + 1, right - left - 1);
        QString hash = line.mid(right + 2);
        // We handle password.
        // If we found user then we put password in table.
        foreach (int row, m_showTableMap.values(hash)) {
            m_hashesTable->setData(
                m_hashesTable->index(row, 1),
                password);
        }
        // We remove value to speed up.
        m_showTableMap.remove(hash);
        // We continue reading with next line.
        line = outputStream.readLine();
    }
    QString lastLine;
    if (!line.isNull()) {
        // We are on the last line.
        // We take counts of cracked passwords and of left hashes.
        // We read count of cracked password hashes.
        lastLine = outputStream.readLine();
    } else {
        lastLine = firstLine;
    }
    QRegExp crackedLeft("(\\d+)\\D+(\\d+)");
    int pos = crackedLeft.indexIn(lastLine);
    if (pos > -1) {
        int crackedCount = crackedLeft.cap(1).toInt();
        int leftCount = crackedLeft.cap(2).toInt();
        // We update progress bar.
        if (crackedCount + leftCount == 0) {
            // There are no hashes.
            m_ui->progressBar->setRange(0, 1);
            m_ui->progressBar->setValue(0);
            m_ui->progressBar->setFormat(
                tr("No hashes loaded [%1], see output").arg(
                    m_format));
        } else {
            m_ui->progressBar->setRange(0, crackedCount + leftCount);
            m_ui->progressBar->setValue(crackedCount);
            m_ui->progressBar->setFormat(
                tr("%p% (%v/%m: %1 cracked, %2 left) [%3]").arg(
                    crackedCount).arg(
                        leftCount).arg(
                            m_format));
        }
    } else {
        // TODO: Error: unexpected john output.
        // TODO: Unknown cyphertext format is here. Read stderr to check exactly.
    }
}

// Settings page code
// NOTE: To add new setting you should add data member to form's
//       class, then you should add copying line to every method on
//       the list:
//       fillSettingsWithDefaults,
//       applySettings,
//       applyAndSaveSettings,
//       restoreLastSavedSettings,
//       And of course you should put elements on the form.
//       And you must connect the signal eventChanged of the widget to the slot
//       settingsChangedByUser() to take care of autoApply setting

void MainWindow::warnAboutDefaultPathToJohn()
{
    QMessageBox::warning(
        this,
        tr("Johnny: default path to john"),
        tr("Currently Johnny filled settings with default path to John the Ripper (%1). "
           "You could set your preferred path in settings "
           "(just use 'john' there to make Johnny search for John the Ripper "
           "in PATH on every invocation of John the Ripper). "
           "If you are satisfied with defaults then save settings to avoid this message.").arg(
               m_ui->comboBox_PathToJohn->currentText()));
}

void MainWindow::fillSettingsWithDefaults()
{
    QStringList possiblePaths;
    QString john;
    // Find john on system path, which is determined by PATH variable
    QString johnSystemPath = QStandardPaths::findExecutable("john", QStringList());
    if(!johnSystemPath.isEmpty())
        possiblePaths << johnSystemPath;

    // Predefined defaults
    // John might be in in the same directory than johnny
    QString johnOtherPaths = QStandardPaths::findExecutable("john", QStringList(QDir::currentPath()));
    if(!johnOtherPaths.isEmpty())
        possiblePaths << johnOtherPaths;

    // Find first readable, executable file from possible
    foreach (QString path, possiblePaths) {
        QFileInfo iJohn(path);
        if (iJohn.isReadable() && iJohn.isExecutable()) {
            john = path;
            break;
        }
    }

    m_ui->comboBox_PathToJohn->setEditText(john);
    m_ui->spinBox_TimeIntervalPickCracked->setValue(INTERVAL_PICK_CRACKED);
    m_ui->checkBox_AutoApplySettings->setChecked(false);
}

void MainWindow::buttonFillSettingsWithDefaultsClicked()
{
    fillSettingsWithDefaults();
    warnAboutDefaultPathToJohn();
}

void MainWindow::buttonBrowsePathToJohnClicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFile);
    if (dialog.exec()) {
        QString fileName = dialog.selectedFiles()[0];
        // We put file name into field for it.
        m_ui->comboBox_PathToJohn->setEditText(fileName);
    }
}

void MainWindow::applySettings()
{
    // We verify john version
    QString newJohnPath = m_ui->comboBox_PathToJohn->currentText();
    if ((m_pathToJohn != newJohnPath) && !newJohnPath.isEmpty()) {
        m_johnVersionCheck.setJohnProgram(newJohnPath);
        m_johnVersionCheck.start();
    }
    // We copy settings from elements on the form to the settings
    // object with current settings.
    m_pathToJohn = newJohnPath;
    m_timeIntervalPickCracked = m_ui->spinBox_TimeIntervalPickCracked->value();
    m_autoApplySettings = m_ui->checkBox_AutoApplySettings->isChecked();

    // If the language changed, retranslate the UI
    Translator& translator = Translator::getInstance();
    QString newLanguage = m_ui->comboBox_LanguageSelection->currentText().toLower();
    if (newLanguage != translator.getCurrentLanguage().toLower()) {
        translator.translateApplication(qApp,newLanguage);
        m_ui->retranslateUi(this);
    }
}

void MainWindow::applyAndSaveSettings()
{
    applySettings();
    m_settings.setValue("PathToJohn", m_ui->comboBox_PathToJohn->currentText());
    m_settings.setValue("TimeIntervalPickCracked", m_ui->spinBox_TimeIntervalPickCracked->value());
    m_settings.setValue("AutoApplySettings", m_ui->checkBox_AutoApplySettings->isChecked());
    m_settings.setValue("Language", m_ui->comboBox_LanguageSelection->currentText().toLower());
}

void MainWindow::restoreSavedSettings()
{
    // We copy stored settings to the form and then invoke applySettings()
    // TODO: Add sensible defaults to all values
    QString settingsPathToJohn = m_settings.value("PathToJohn").toString();
    m_ui->comboBox_PathToJohn->setEditText(
        settingsPathToJohn == ""
        ? m_ui->comboBox_PathToJohn->currentText()
        : settingsPathToJohn);
    m_ui->spinBox_TimeIntervalPickCracked->setValue(
        m_settings.value("TimeIntervalPickCracked").toString() == ""
        ? m_ui->spinBox_TimeIntervalPickCracked->value()
        : m_settings.value("TimeIntervalPickCracked").toInt());
    m_ui->checkBox_AutoApplySettings->setChecked(
        m_settings.value("AutoApplySettings").toString() == ""
        ? m_ui->checkBox_AutoApplySettings->isChecked()
        : m_settings.value("AutoApplySettings").toBool());
    applySettings();
}

// Handlers for settings auto application

void MainWindow::settingsChangedByUser()
{
    if (m_autoApplySettings)
        applySettings();
}

void MainWindow::checkBoxAutoApplySettingsStateChanged()
{
    // First goal is to disable 'apply' button and to apply settings
    // when auto application is turned on.
    bool autoApply = m_ui->checkBox_AutoApplySettings->isChecked();
    m_ui->pushButton_ApplySettings->setEnabled(!autoApply);
    if (autoApply)
        applySettings();
    // Second goal is auto application for auto application setting itself.
    // NOTE: Deactivation of auto application will be auto applied.
    // NOTE: Auto application is a setting too. At least it would be
    //       good to remember its state between program runs.
    if (m_autoApplySettings)
        applySettings();
}


// Statistics page code

void MainWindow::updateStatistics()
{
    // Working time
    // We could not just subtract one time from another. But we could
    // know days and seconds between two time points.
    //
    // We check whether John is running.
    if (m_johnAttack.state() == QProcess::Running) {
        // If John is running then we put time of its work on the
        // form.
        // We remember current time.
        QDateTime currentDateTime = QDateTime::currentDateTime();
        // We count days and seconds since attack start.
        int days = m_startDateTime.daysTo(currentDateTime),
            seconds = m_startDateTime.secsTo(currentDateTime);
        // We compute minutes and hours since attack start.
        int minutes = seconds / 60 % 60,
            hours = seconds / 60 / 60 % 24;
        // We modify seconds value to be shorter than minute.
        seconds %= 60;
        // We produce a string representing distance between time points.
        QString workingTime;
        QTextStream stream(&workingTime);

        stream << days << tr(":");
        // Hours, minutes and seconds have padding with zeroes to two
        // chars.
        stream.setPadChar('0');
        stream.setFieldAlignment(QTextStream::AlignRight);
        stream << qSetFieldWidth(2) << hours << qSetFieldWidth(1) << tr(":");
        stream << qSetFieldWidth(2) << minutes << qSetFieldWidth(1) << tr(":");
        stream << qSetFieldWidth(2) << seconds;
        // We put prepared string on the form.
        m_ui->label_StatisticsWorkingTime->setText(workingTime);
    } else {
        // Else (if John is not running) we put dash instead of time.
        m_ui->label_StatisticsWorkingTime->setText(tr("-"));
    }
}

/*
 * Since QPlainTextEdit::appendPlainText() add newLines without asking us and
 * QPlainTextEdit::insertPlainText() insert text by default at the cursor pos,
 * which can be modified by the user, this function assures the text is
 * inserted at the end without new line by default.
 */

void MainWindow::appendLog(const QString& text)
{
    // Preserving cursor preserves selection by user
    QTextCursor prev_cursor = m_ui->plainTextEdit_JohnOut->textCursor();
    m_ui->plainTextEdit_JohnOut->moveCursor(QTextCursor::End);
    m_ui->plainTextEdit_JohnOut->insertPlainText(text);
    m_ui->plainTextEdit_JohnOut->setTextCursor(prev_cursor);
}

/* This slot is triggered when the types changed. This is probably because :
 * 1) a new password file has been loaded OR 2) old file with a new jumbo john was used
 */
void MainWindow::updateHashTypes(const QStringList &pathToPwdFile, const QStringList &listOfTypesInFile,
                                 const QStringList &detailedTypesPerRow)
{
    FileTableModel* model = dynamic_cast<FileTableModel*>(m_hashesTable);
    if ((model != NULL) && (pathToPwdFile == m_passwordFiles)) {
        // We know that the right file is still opened so the signal
        // isn't too late, otherwise we don't replace the model
        model->fillHashTypes(detailedTypesPerRow);
        m_ui->tableView_Hashes->setModel(model);
        QString savedFormat = m_ui->comboBox_Format->currentText();
        // For jumbo, we list only available formats in file in attack option
        m_ui->comboBox_Format->clear();
        m_ui->comboBox_Format->addItem(tr("Auto detect"));
        m_ui->comboBox_Format->addItems(listOfTypesInFile);
        // Restore user's selection
        int indexSavedFormat = m_ui->comboBox_Format->findText(savedFormat);
        if (indexSavedFormat != -1) {
            m_ui->comboBox_Format->setCurrentIndex(indexSavedFormat);
        } else if(savedFormat.isEmpty()){
            m_ui->comboBox_Format->setEditText(savedFormat);
        }
    }
}

// Enable/Disable all features that are jumbo related in this method
void MainWindow::setAvailabilityOfFeatures(bool isJumbo)
{
    bool wasLastVersionJumbo = m_isJumbo;
    m_isJumbo = isJumbo;
    if ((wasLastVersionJumbo == false) && (isJumbo == true) && (!m_passwordFiles.isEmpty())) {
        m_hashTypeChecker.setJohnProgram(m_pathToJohn);
        m_hashTypeChecker.setPasswordFiles(m_passwordFiles);
        m_hashTypeChecker.start();
    }
    m_ui->tableView_Hashes->setColumnHidden(FileTableModel::FORMATS_COL, !isJumbo);
    if (!isJumbo) {
        // Add default format list supported by core john
        QStringList defaultFormats;
        defaultFormats << tr("Auto detect") << "descrypt" << "bsdicrypt" << "md5crypt"
                       << "bcrypt" << "AFS" << "LM" << "crypt" << "tripcode" << "dummy";
        QString savedFormat = m_ui->comboBox_Format->currentText();
        m_ui->comboBox_Format->clear();
        m_ui->comboBox_Format->addItems(defaultFormats);
        // Restore user's selection
        int indexSavedFormat = m_ui->comboBox_Format->findText(savedFormat);
        if (indexSavedFormat != -1) {
            m_ui->comboBox_Format->setCurrentIndex(indexSavedFormat);
        } else if (!savedFormat.isEmpty()){
            m_ui->comboBox_Format->setEditText(savedFormat);
        }
    }
}

void MainWindow::verifyJohnVersion()
{
    QString output = m_johnVersionCheck.readAllStandardOutput();
    bool isJumbo = output.contains("jumbo", Qt::CaseInsensitive);
    setAvailabilityOfFeatures(isJumbo);
}

void MainWindow::actionOpenSessionTriggered(QAction* action)
{
    if ((action == m_ui->actionClearSessionHistory) && !m_sessionHistory.isEmpty()) {
        QDir dir(m_appDataPath);
        dir.setNameFilters(QStringList() << "*.log" << "*.johnny" << "*.rec");
        dir.setFilter(QDir::Files);
        foreach (QString dirFile, dir.entryList()) {
            dir.remove(dirFile);
        }
        foreach(QAction* actions, m_sessionMenu->actions()) {
            if (m_sessionHistory.removeOne(actions->data().toString())) {
                    m_sessionMenu->removeAction(actions);
            }
        }
        m_session.clear();
        m_settings.remove("johnSessions");
        m_ui->actionResumeAttack->setEnabled(false);
    } else {
        QString fileName = action->data().toString();
        if (!fileName.isEmpty()) {
            m_session = QDir(m_appDataPath).filePath(fileName);
            openLastSession();
        }
    }
}

void MainWindow::guessPassword()
{
    bool isOk;
    QString guess = QInputDialog::getText(this, tr("Password Guessing"),
                                         tr("Your passphrase guess:"), QLineEdit::Normal,
                                         "", &isOk);
    if (isOk && !guess.isEmpty()) {
        m_ui->actionGuessPassword->setEnabled(false);
        m_passwordGuessing.setJohnProgram(m_pathToJohn);
        m_passwordGuessing.setArgs(QStringList() << "--stdin" << "--session=passwordGuessing" << m_passwordFiles);
        m_passwordGuessing.start();
        m_passwordGuessing.write(guess);
        m_passwordGuessing.closeWriteChannel();
    }
}

void MainWindow::guessPasswordFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    m_ui->actionGuessPassword->setEnabled(true);
    if (exitStatus == QProcess::CrashExit) {
        qDebug() << "JtR seems to have crashed.";
        return;
    }
}

void MainWindow::restoreSessionOptions()
{
    restoreDefaultAttackOptions();
    // Start restoring required UI fields
    QString sessionName(m_session);
    sessionName.remove(m_appDataPath);
    m_settings.beginGroup("johnSessions/" + sessionName);
    m_format = m_settings.value("formatJohn").toString();
    m_ui->comboBox_Format->setEditText(m_settings.value("formatUI").toString());
    QString mode = m_settings.value("mode").toString();
    if (mode == "single") {
        m_ui->attackModeTabWidget->setCornerWidget(m_ui->singleModeTab);
        // External mode, filter
        if(m_settings.contains("singleCrackExternalName")) {
            m_ui->checkBox_SingleCrackModeExternalName->setChecked(true);
            m_ui->comboBox_SingleCrackModeExternalName->setEditText(m_settings.value("singleCrackExternalName").toString());
        }      
    } else if (mode == "wordlist") {
        m_ui->attackModeTabWidget->setCurrentWidget(m_ui->wordlistModeTab);  
        m_ui->comboBox_WordlistFile->setEditText(m_settings.value("wordlistFile").toString());
        //Rules
        if (m_settings.value("isUsingWordListRules").toBool() == true) {
            m_ui->checkBox_WordlistModeRules->setChecked(true);
        }
        // External mode, filter
        if (m_settings.contains("worldListExternalName")) {
            m_ui->checkBox_WordlistModeExternalName->setChecked(true);
            m_ui->comboBox_WordlistModeExternalName->setEditText(m_settings.value("worldListExternalName").toString());
        }
    } else if (mode == "incremental") {
        m_ui->attackModeTabWidget->setCurrentWidget(m_ui->incrementalModeTab);
        // "Incremental" mode
        // It could be with or without name.
        if (m_settings.contains("incrementalModeName")) {
            m_ui->checkBox_IncrementalModeName->setChecked(true);
            m_ui->comboBox_IncrementalModeName->setEditText(m_settings.value("incrementalModeName").toString());
        }
        // External mode, filter
        if (m_settings.contains("incrementalExternalName")) {
            m_ui->checkBox_IncrementalModeExternalName->setChecked(true);
            m_ui->comboBox_IncrementalModeExternalName->setEditText(m_settings.value("incrementalExternalName").toString());
        }  
    } else if (mode == "external") {
        m_ui->attackModeTabWidget->setCurrentWidget(m_ui->externalModeTab)  ;
        m_ui->comboBox_ExternalModeName->setEditText(m_settings.value("externalModeName").toString());
    } else {
        m_ui->attackModeTabWidget->setCurrentWidget(m_ui->defaultModeTab);
    }
    
    // Selectors
    if (m_settings.contains("limitUsers")) {
        m_ui->checkBox_LimitUsers->setChecked(true);
        m_ui->comboBox_LimitUsers->setEditText(m_settings.value("limitUsers").toString());
    }
    if (m_settings.contains("limitGroups")) {
        m_ui->checkBox_LimitGroups->setChecked(true);
        m_ui->comboBox_LimitGroups->setEditText(m_settings.value("limitGroups").toString());
    }
    if (m_settings.contains("limitShells")) {
        m_ui->checkBox_LimitShells->setChecked(true);
        m_ui->comboBox_LimitShells->setEditText(m_settings.value("limitShells").toString());
    }
    if (m_settings.contains("limitSalts")) {
        m_ui->checkBox_LimitSalts->setChecked(true);
        m_ui->spinBox_LimitSalts->setValue(m_settings.value("limitSalts").toInt());
    }

    // Advanced options
    if (m_settings.contains("nbForkProcess")) {
        m_ui->checkBox_UseFork->setChecked(true);
        int nbOfProcess = m_settings.value("nbForkProcess").toInt();
        // In case the restored session ideal thread count is greather than current maximum (ex: user changed VM settings),
        // we have to restore the right previous session value.
        if (nbOfProcess > m_ui->spinBox_nbOfProcess->maximum()) {
            m_ui->spinBox_nbOfProcess->setMaximum(nbOfProcess);
        }
        m_ui->spinBox_nbOfProcess->setValue(nbOfProcess);
    }
    m_ui->spinBox_nbOfOpenMPThread->setValue(m_settings.value("OMP_NUM_THREADS").toInt());
    
    if (m_settings.contains("environmentVariables")) {
        m_ui->checkBox_EnvironmentVar->setChecked(true);
        m_ui->lineEdit_EnvironmentVar->setText(m_settings.value("environmentVariables").toString());
    }
    m_settings.endGroup();
}

/* Clear/or default optional previous session UI options that may 
   not be specified in the settings depending on the mode
*/
void MainWindow::restoreDefaultAttackOptions(bool shouldClearFields)
{
    if (shouldClearFields) {
        foreach(QCheckBox *widget, m_ui->optionsPage->findChildren<QCheckBox*>()) {
            widget->setChecked(false);
        }
        foreach(QComboBox *widget, m_ui->optionsPage->findChildren<QComboBox*>()) {
            widget->setCurrentText("");
        }
    }
    m_ui->spinBox_nbOfProcess->setMaximum(QThread::idealThreadCount());
    m_ui->spinBox_nbOfProcess->setValue(QThread::idealThreadCount());
    m_ui->spinBox_nbOfProcess->setMinimum(2); // john --fork will error if < 2, let's prevent it
    m_ui->spinBox_LimitSalts->setValue(0);        
    m_ui->attackModeTabWidget->setCurrentWidget(m_ui->defaultModeTab);
    m_ui->spinBox_nbOfOpenMPThread->setValue(0); // 0 means special value = default
}
