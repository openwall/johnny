/*
 * Copyright (c) 2011 Shinnok <raydenxy at gmail.com>.
 * Copyright © 2011,2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>.  See LICENSE.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

// We include all table models we use.
#include "tablemodel.h"
#include "filetablemodel.h"

#include <QToolButton>
#include <QStringListModel>
#include <QFileDialog>
#include <QFile>
#include <QByteArray>
#include <QTextStream>
#include <QMessageBox>
#include <QClipboard>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_ui(new Ui::MainWindow),
      m_hashesTable(NULL),
      m_settings(
          // TODO: is not .ini better than .conf?
          QDir(QDir::home().filePath(".john")).filePath("johnny.conf"),
          QSettings::IniFormat),
      m_temp(NULL)
{
    m_ui->setupUi(this);

    // TODO: Magic number. It seems to be wrong approach.
    m_ui->listWidgetTabs->setMaximumWidth(m_ui->listWidgetTabs->sizeHintForColumn(0) + 4);

    // TODO: How to select Passwords tab through mainwindow.ui?
    //       Property 'selected' with 'bool' value 'true' did not work for me.
    // We select first item/tab on list.
    m_ui->listWidgetTabs->setCurrentRow(0);

    // TODO: Is it possible to make it through mainwindow.ui?
    // TODO: Does this find condition works for all strings?
    foreach (QListWidgetItem *item, m_ui->listWidgetTabs->findItems("", Qt::MatchContains))
        item->setSizeHint(QSize(m_ui->listWidgetTabs->sizeHintForColumn(0), 60));

    /*
    // We add a button to the toolbar but this button is not simple. It has
    // menu. And that menu drops like from menu button. Just QAction could not
    // do it. Just QPushButton looses it's main action for popping
    // menu. Solution with the best looking is QToolButton. QtCreator could not
    // put such element on tool bar. It is possible to put it by hands through
    // .ui file but it does not seem to be reliable. So we make it here in
    // code.
    //
    // We create a desired menu.
    QMenu *sessionMenu = new QMenu(this);
    sessionMenu->addAction(m_ui->actionNew_Session);
    sessionMenu->addAction(m_ui->actionSave_Session);
    // We create a button.
    QToolButton *sessionMenuButton = new QToolButton(this);
    // We set default action and menu for the button.
    sessionMenuButton->setDefaultAction(m_ui->actionOpen_Session);
    sessionMenuButton->setMenu(sessionMenu);
    // We set button up to have desired look and behaviour.
    sessionMenuButton->setPopupMode(QToolButton::MenuButtonPopup);
    // TODO: May it be better to derive this setting from the toolbar?
    sessionMenuButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    // We put the button onto the toolbar.
    m_ui->mainToolBar->insertWidget(m_ui->actionOpen_Password, sessionMenuButton);
    */

    // TODO: Could we make connections easier?
    // We connect John process' signals with our slots.
    // John was ended.
    // TODO: It will good to show exit status and exit code to user.
    connect(&m_johnProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(showJohnFinished()));
    // John was started.
    connect(&m_johnProcess, SIGNAL(started()),
            this, SLOT(showJohnStarted()));
    // John got problem.
    // TODO: catch problems from `john --show`?
    connect(&m_johnProcess, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(showJohnError(QProcess::ProcessError)));
    // John wrote something.
    connect(&m_johnProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(updateJohnOutput()));
    connect(&m_johnProcess, SIGNAL(readyReadStandardError()),
            this, SLOT(updateJohnOutput()));

    // We connect timer with calling for john to show us status.
    connect(&m_showTimer, SIGNAL(timeout()),
            this, SLOT(callJohnShow()));
    // We connect 'john --show' process with our object.
    connect(&m_showJohnProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(readJohnShow()));

    // We connect all widgets for option with radio buttons to keep
    // enabled only widgets of selected mode.
    // TODO: Easier way? Maybe make connections in designer?
    // "Single crack" mode
    connect(m_ui->radioButton_SingleCrackMode, SIGNAL(toggled(bool)),
            m_ui->checkBox_SingleCrackModeExternalName, SLOT(setEnabled(bool)));
    // TODO: Should not we keep comboBox disabled until checkbox would
    //       not be checked? (in other places too)
    connect(m_ui->radioButton_SingleCrackMode, SIGNAL(toggled(bool)),
            m_ui->comboBox_SingleCrackModeExternalName, SLOT(setEnabled(bool)));
    // Wordlist mode
    connect(m_ui->radioButton_WordlistMode, SIGNAL(toggled(bool)),
            m_ui->label_WordlistFile, SLOT(setEnabled(bool)));
    connect(m_ui->radioButton_WordlistMode, SIGNAL(toggled(bool)),
            m_ui->comboBox_WordlistFile, SLOT(setEnabled(bool)));
    connect(m_ui->radioButton_WordlistMode, SIGNAL(toggled(bool)),
            m_ui->pushButton_WordlistFileBrowse, SLOT(setEnabled(bool)));
    connect(m_ui->radioButton_WordlistMode, SIGNAL(toggled(bool)),
            m_ui->checkBox_WordlistModeRules, SLOT(setEnabled(bool)));
    // connect(m_ui->radioButton_WordlistMode, SIGNAL(toggled(bool)),
    //         m_ui->checkBox_WordlistModeRulesName, SLOT(setEnabled(bool)));
    // connect(m_ui->radioButton_WordlistMode, SIGNAL(toggled(bool)),
    //         m_ui->comboBox_WordlistModeRulesName, SLOT(setEnabled(bool)));
    connect(m_ui->radioButton_WordlistMode, SIGNAL(toggled(bool)),
            m_ui->checkBox_WordlistModeExternalName, SLOT(setEnabled(bool)));
    connect(m_ui->radioButton_WordlistMode, SIGNAL(toggled(bool)),
            m_ui->comboBox_WordlistModeExternalName, SLOT(setEnabled(bool)));
    // Incremental mode
    connect(m_ui->radioButton_IncrementalMode, SIGNAL(toggled(bool)),
            m_ui->checkBox_IncrementalModeName, SLOT(setEnabled(bool)));
    connect(m_ui->radioButton_IncrementalMode, SIGNAL(toggled(bool)),
            m_ui->comboBox_IncrementalModeName, SLOT(setEnabled(bool)));
    connect(m_ui->radioButton_IncrementalMode, SIGNAL(toggled(bool)),
            m_ui->checkBox_IncrementalModeExternalName, SLOT(setEnabled(bool)));
    connect(m_ui->radioButton_IncrementalMode, SIGNAL(toggled(bool)),
            m_ui->comboBox_IncrementalModeExternalName, SLOT(setEnabled(bool)));
    // External mode
    connect(m_ui->radioButton_ExternalMode, SIGNAL(toggled(bool)),
            m_ui->label_ExternalModeName, SLOT(setEnabled(bool)));
    connect(m_ui->radioButton_ExternalMode, SIGNAL(toggled(bool)),
            m_ui->comboBox_ExternalModeName, SLOT(setEnabled(bool)));

    // To open respective tab on mode selection
    QButtonGroup *group = m_ui->radioButton_ExternalMode->group();
    // TODO: Maybe cycle? Maybe put this into .ui?
    group->setId(m_ui->radioButton_DefaultBehaviour, 0);
    group->setId(m_ui->radioButton_SingleCrackMode, 1);
    group->setId(m_ui->radioButton_WordlistMode, 2);
    group->setId(m_ui->radioButton_IncrementalMode, 3);
    group->setId(m_ui->radioButton_ExternalMode, 4);
    connect(group, SIGNAL(buttonClicked(int)),
            m_ui->tabWidget, SLOT(setCurrentIndex(int)));

//    TableModel *passmodel = new TableModel();

//    ui->tableView_Passwords->setModel(passmodel);

//    for (int i = 0; i < TABLE_ROWS; i++) {
//        passmodel->setData(passmodel->index(i, 0), QString("Rick%1").arg(i));
//        passmodel->setData(passmodel->index(i, 1), QString("Never gonna give you up!"));
//    }


    // We create folder for us in home dir if it does not exist.
    // TODO: Are this checks are enough?
    // TODO: Claim on mkdir fails.
    // TODO: Do not do it on start up. Choose other good time.
    if (!QDir(QDir::home().filePath(".john")).exists()) {
        QDir::home().mkdir(".john");
    }
    if (!QDir(QDir(QDir::home().filePath(".john")).filePath("johnny")).exists()) {
        QDir(QDir::home().filePath(".john")).mkdir("johnny");
    }

    // Session for johnny
    m_session = QDir(
        QDir(QDir::home().filePath(
                 ".john")).filePath(
                     "johnny")).filePath(
                         "default");

    checkNToggleActionsLastSession();

    // We fill form with default values. Then we load settings. When
    // there is no setting old value is used. So if there is no
    // configuration file then we get default values. Also it means
    // that when user pushes "reset to stored settings" button he
    // resets only fields with values. It is nice but may seem
    // strange: probably user would expect reset to be like default +
    // reset because storing of a part of settings is not normal
    // behaviour (only possible with upgrades).
    // TODO: somewhat ugly.
    // TODO: if there are no config or it is partial then claim. Do
    //       not silently do something tricky.
    // TODO: do not search john if it is in stored settings. It is
    //       similar to other settings. It is at least start up speed up.
    fillSettingsWithDefaults();

    // We load old settings.
    // TODO: bad name.
    on_pushButton_ResetSettings_clicked();

    // TODO: do this message on every invocation of john. Provide
    //       checkbox to not show this again.
    // TODO: default values for other settings are accepted silently.
    // if (m_settings.value("PathToJohn").toString() == "")
    //     warnAboutDefaultPathToJohn();

}

void MainWindow::checkNToggleActionsLastSession()
{
    m_ui->actionStart_Attack->setEnabled(m_hashesFileName != "");

    if (QFileInfo(m_session + ".rec").isReadable()
        && QFileInfo(m_session + ".johnny").isReadable()) {
        m_ui->actionOpen_Last_Session->setEnabled(true);

        // TODO: very similar to open last session code.
        QFile description(m_session + ".johnny");
        if (!description.open(QIODevice::ReadOnly | QIODevice::Text)) {
            m_ui->actionResume_Attack->setEnabled(false);
            return;
        }
        QTextStream descriptionStream(&description);
        // TODO: errors?
        QString hashesFileName = descriptionStream.readLine();
        description.close();

        m_ui->actionResume_Attack->setEnabled(
            hashesFileName == m_hashesFileName
            && hashesFileName != "");
        m_ui->actionOpen_Last_Session->setEnabled(
            hashesFileName != m_hashesFileName);
    } else {
        m_ui->actionOpen_Last_Session->setEnabled(false);
        m_ui->actionResume_Attack->setEnabled(false);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_johnProcess.state() != QProcess::NotRunning) {
        int answer = QMessageBox::question(
            this,
            tr("Johnny"),
            // TODO: More informative message.
            tr("John still runs! John will be stopped if you proceed. Do you really want to quit?"),
            QMessageBox::Yes | QMessageBox::No);
        if (answer == QMessageBox::No) {
            event->ignore();
            return;
        }
    }
    event->accept();
}

MainWindow::~MainWindow()
{
    m_johnProcess.terminate();
    m_showJohnProcess.terminate();
    // TODO: we wait 1 second.
    if (!m_johnProcess.waitForFinished(1000))
        m_johnProcess.kill();
    if (!m_showJohnProcess.waitForFinished(1000))
        m_showJohnProcess.kill();
    delete m_ui;
    m_ui = 0;
    delete m_hashesTable;
    m_hashesTable = 0;
    delete m_temp;
    m_temp = 0;
}

void MainWindow::on_pushButton_WordlistFileBrowse_clicked()
{
    // We pop a dialog to choose a file to open.
    // TODO: *.lst for file extension?
    // TODO: Copy-pasting is evil! (open password file)
    // TODO: What happens when John writes something while dialog
    //       opened?
    // TODO: Move dialog creation and setting up into window constructor.
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFile);
    // TODO: Dialog could allow user to select multiple files. May it
    //       be good to support this ability? To concatenate selected file?
    if (dialog.exec()) {
        QString fileName = dialog.selectedFiles()[0];
        // We put file name into field for it.
        // TODO: File name does not appear in history (drop down list).
        m_ui->comboBox_WordlistFile->setEditText(fileName);
    }
}

void MainWindow::on_listWidgetTabs_itemSelectionChanged()
{
    m_ui->stackedWidget->setCurrentIndex(m_ui->listWidgetTabs->currentRow());
    // TODO: We assume here that passwords tab is at 0.
    m_ui->actionCopyToClipboard->setEnabled(m_ui->listWidgetTabs->currentRow() == 0);
}

void MainWindow::replaceTableModel(QAbstractTableModel *newTableModel)
{
    // TODO: Check argument.

    // Remove temporary file is exist
    // TODO: should not I check pointers like ptr != NULL instead of just ptr?
    // TODO: should not I assign NULL instead of 0?
    if (m_temp) {
        delete m_temp;
        m_temp = 0;
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

    // We build hash table for fast access.
    m_tableMap = QMultiMap<QString, int>();
    for (int i = 0; i < m_hashesTable->rowCount(); i++) {
        m_tableMap.insert(
            m_hashesTable->data(m_hashesTable->index(i, 2)).toString(),
            i);
    }
}

// void MainWindow::on_pushButton_clicked()
// {
//     replaceTableModel(new TableModel(this));
//     // We reset file name because this model is generated and does not
//     // have connected file.
//     m_hashesFileName = "";

//     for (int i = 0; i < TABLE_ROWS; i++) {
//         m_hashesTable->setData(m_hashesTable->index(i, 0), QString("Rick%1").arg(i));
//         m_hashesTable->setData(m_hashesTable->index(i, 1), QString("6817f89c171a439b3d0418a18a236001"));
//     }
// }

bool MainWindow::readPasswdFile(const QString &fileName)
{
    FileTableModel *model = new FileTableModel(this);
    if (model->readFile(fileName)) {
        // We replace existing model with new one.
        replaceTableModel(model);
        // After new model remembered we remember its file name.
        m_hashesFileName = fileName;
        checkNToggleActionsLastSession();
        return true;
    }
    QMessageBox::warning(
            this,
            tr("Johnny"),
            // TODO: More informative message.
            tr("Johnny could not read desired passwd file."));
    return false;
}

void MainWindow::on_actionOpen_Password_triggered()
{
    // When user asks to open password file we should read desired
    // file, parse it and present values in the table. Model and view
    // simplifies presentation. We just make and fill model and then
    // we set it to existing view.
    // TODO: However there are variants: we could replace existing
    //       model or append new values to existing model.
    //       For now user could not choose what to do. We always
    //       replace existing model.

    // We pop a dialog to choose a file to open.
    // TODO: What happens when John writes something while dialog
    //       opened?
    // TODO: Move dialog creation and setting up into window constructor.
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFile);
    // TODO: Dialog could allow user to select multiple files. May it
    //       be good to support this ability? There are variants: to
    //       concatenate selected file or to unshadow them.
    if (dialog.exec()) {
        QString fileName = dialog.selectedFiles()[0];
        readPasswdFile(fileName);
    }
}

void MainWindow::on_actionOpen_Last_Session_triggered()
{
    QFile description(m_session + ".johnny");
    if (!description.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(
            this,
            tr("Johnny"),
            // TODO: More informative message.
            tr("Johnny could not open file to read session description!"));
        return;
    }
    QTextStream descriptionStream(&description);
    // TODO: errors?
    // TODO: end of line? Should not we "chomp" it? See other places too.
    QString fileName = descriptionStream.readLine();
    QString format = descriptionStream.readLine();
    description.close();
    if (readPasswdFile(fileName)) {
        m_format = format;
    }
}

void MainWindow::on_actionCopyToClipboard_triggered()
{
    if (!m_hashesTable)
        return;
    QModelIndexList indexes = m_ui->tableView_Hashes->selectionModel()->selectedIndexes();
    // TODO: maybe copy everything if nothing is selected?
    if (indexes.count() == 0)
        return;
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
    // TODO: we copy in two modes. I guess only one should be used.
    //       Selection mode on linux and clipboard on windows?
    if (clipboard->supportsSelection())
        clipboard->setText(out, QClipboard::Selection);
    clipboard->setText(out);
}

bool MainWindow::checkSettings()
{
    if (m_pathToJohn == "") {
        QMessageBox::critical(
            this,
            tr("Johnny"),
            tr("Please specify path to John the Ripper binary in settings!"));
        return false;
    }
    return true;
}

void MainWindow::on_actionStart_Attack_triggered()
{
    if (!checkSettings())
        return;

    QStringList parameters;
    // We prepare parameters list from options section.
    // TODO: Make it as separate method. It also will be needed to
    //       preview command line that not done now.
    // General options
    // Format
    // TODO: If format is with checkbox then 'auto detect' does
    //       not exist and format is handled as other options.
    //       It could give an ability to unify all options and keep
    //       them not as code.
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
        // TODO: What is better format to use for keys: -key: or
        //       --key= or something else?
        // We remember format key to be used with '-show' to take
        // progress.
        // TODO: Instead of remembering of keys we could lock options.
        //       What is better?
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
        m_format = "";
    }
    // Modes
    if (m_ui->radioButton_DefaultBehaviour->isChecked()) {
        // Default behaviour - no modes
        // There are no options here.
    } else if (m_ui->radioButton_SingleCrackMode->isChecked()) {
        // "Single crack" mode
        parameters << "--single";
        // External mode, filter
        // TODO: It is applicable for 3 formats. Copy-pasting is evil!
        // TODO: Warn if checkbox is checked and there is not text in
        //       combobox. For other empty fields it would great to
        //       warn too.
        if (m_ui->checkBox_SingleCrackModeExternalName->isChecked())
            parameters << ("--external=" + m_ui->comboBox_SingleCrackModeExternalName->currentText());
    } else if (m_ui->radioButton_WordlistMode->isChecked()) {
        // Wordlist mode
        parameters << ("--wordlist=" + m_ui->comboBox_WordlistFile->currentText());
        // Rules
        // They could appear with or without name.
        if (m_ui->checkBox_WordlistModeRules->isChecked()) {
            // If rules are selected then we distinguish either name
            // is needed two.
            // if (m_ui->checkBox_WordlistModeRulesName->isChecked()) {
            //     // If name for rules is selected to be then we take it.
            //     // NOTE: Not all versions support this.
            //     // TODO: It would be great to notice user about this.
            //     // TODO: It would be great to notice user on any
            //     //       fails, not only here.
            //     // TODO: Calls to John makes interface to stick a bit.
            //     //       It is actual with often -show calls.
            //     parameters << ("--rules=" + m_ui->comboBox_WordlistModeRulesName->currentText());
            // } else {
                // If no name is needed then we use just rules,
                // without name.
                parameters << "--rules";
            // }
        }
        // External mode, filter
        if (m_ui->checkBox_WordlistModeExternalName->isChecked())
            parameters << ("--external=" + m_ui->comboBox_WordlistModeExternalName->currentText());
    } else if (m_ui->radioButton_IncrementalMode->isChecked()) {
        // "Incremental" mode
        // It could be with or without name.
        if (m_ui->checkBox_IncrementalModeName->isChecked()) {
            // With name
            parameters << ("--incremental=" + m_ui->comboBox_IncrementalModeName->currentText());
        } else {
            // Without name
            parameters << "--incremental";
        }
        // External mode, filter
        if (m_ui->checkBox_IncrementalModeExternalName->isChecked())
            parameters << ("--external=" + m_ui->comboBox_IncrementalModeExternalName->currentText());
    } else if (m_ui->radioButton_ExternalMode->isChecked()) {
        // External mode
        parameters << ("--external=" + m_ui->comboBox_ExternalModeName->currentText());
    }

    // Selectors
    if (m_ui->checkBox_LimitUsers->isChecked())
        parameters << ("--users=" + m_ui->comboBox_LimitUsers->currentText());
    if (m_ui->checkBox_LimitGroups->isChecked())
        parameters << ("--groups=" + m_ui->comboBox_LimitGroups->currentText());
    if (m_ui->checkBox_LimitShells->isChecked())
        parameters << ("--shells=" + m_ui->comboBox_LimitShells->currentText());
    if (m_ui->checkBox_LimitSalts->isChecked())
        parameters << (QString("--salts=%1").arg(m_ui->spinBox_LimitSalts->value()));

    // Session for johnny
    if (QFileInfo(m_session + ".rec").isReadable()) {
        int button = QMessageBox::question(
            this,
            tr("Johnny"),
            tr("Johnny is about to overwrite your previous session file. Do you want to proceed?"),
            QMessageBox::Yes | QMessageBox::No);
        if (button == QMessageBox::No)
            return;
        // Remove .rec file to avoid problem when john does not write it.
        // TODO: Should not we say something if/when we could not remove file?
        QFile(m_session + ".rec").remove();
    }

    // TODO: Saving so two instances of johnny overwrite description
    //       but not .rec so they become not synchronized.
    QFile description(m_session + ".johnny");
    if (!description.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(
            this,
            tr("Johnny"),
            // TODO: More informative message.
            tr("Johnny could not open file to save session description!"));
        return;
    }
    QTextStream descriptionStream(&description);
    // TODO: john stores file name so itself. It could be changed
    //       later though.
    // TODO: ensure to convert file name into absolute path.
    descriptionStream << m_hashesFileName << endl;
    descriptionStream << m_format << endl;
    description.close();

    // TODO: Easier way is to cd to ~/.john/johnny but it needs
    //       checks. In any case without that dir it will not work.
    parameters << QString("--session=%1").arg(m_session);

    // We check that we have file name.
    if (m_hashesFileName != "") {
        // If file name is not empty then we have file, pass it to
        // John.
        // We add file name onto parameters list.
        parameters << m_hashesFileName;
        startJohn(parameters);
    } else {
        // Else we do not have connected file name so we ask user to save
        // file.
        // TODO: Unreachable until we get fileless tables.
        // TODO: Do something here.
    }
}

void MainWindow::startJohn(QStringList params)
{
    // To start John we have predefined process object. That object's
    // signals are already connected with our slots. So we need only
    // start it.
    //
    // We start John.
    m_johnProcess.start(m_pathToJohn, params);
    // We remember date and time of the start.
    m_startDateTime = QDateTime::currentDateTime();
}

void MainWindow::on_actionResume_Attack_triggered()
{
    if (!checkSettings())
        return;

    QStringList parameters;
    parameters << QString("--restore=%1").arg(m_session);

    startJohn(parameters);
}

void MainWindow::updateJohnOutput()
{
    // TODO: There was a session string passed here. Hence the
    //       question what is right: one window could have multiple sessions or at one
    //       time there could be only one session opened?
    // NOTE: If there could be only one session in/per window then it
    //       is possible to have session name here through window's field.
    // TODO: Session name should be displayed.
    //ui->plainTextEdit_JohnOut->insertPlainText("Session file: " + session + "\n");
    m_ui->plainTextEdit_JohnOut->insertPlainText(m_johnProcess.readAllStandardOutput()); // read output buffer
    m_ui->plainTextEdit_JohnOut->insertPlainText(m_johnProcess.readAllStandardError()); // read error buffer
    // NOTE: Probably here we want to parse John's output, catch newly
    //       cracked passwords and so on. However John's output is buffered.
    //       So we do not obtain it as soon as it occurs. Timer and
    //       --show work for us.
}

void MainWindow::on_actionPause_Attack_triggered()
{
    // We ask John to exit.
    // TODO: Is it ok to call it even if process is not running?
    // TODO: Do not we need to call kill instead if John is too busy?
    // TODO: Call kill on windows.
    // NOTE: We could leave it for user: we count button presses and for
    //       the first time we call terminate and for next times we
    //       call kill.
    m_johnProcess.terminate();
}

// void MainWindow::on_pushButton_JohnStatus_clicked()
// {
//     // When we want to get John status we send enter to John. Then
//     // John write something to its stdout. We do not need to read its
//     // output here because when output is ready to be read a signal is
//     // fired and we read John output with status as any other John's
//     // output.
//     // TODO: However it does not work as of we do not have terminal.
//     // TODO: Why do we write to John even when it is not running?
//     m_johnProcess.write("a\r\n");
// }

void MainWindow::showJohnStarted()
{
    // We disable all buttons.
    m_ui->actionPause_Attack->setEnabled(false);
    m_ui->actionStart_Attack->setEnabled(false);
    m_ui->actionResume_Attack->setEnabled(false);
    m_ui->actionOpen_Password->setEnabled(false);
    m_ui->actionOpen_Last_Session->setEnabled(false);

    // We make a file with original hash in gecos to connect password
    // with original hash during `john --show`.
    if (!m_temp) {
        m_temp = new QTemporaryFile();
        // TODO: check return from new.
        if (m_temp->open()) {
            QTextStream temp(m_temp);
            for (int i = 0; i < m_hashesTable->rowCount(); i++) {
                // TODO: could it be done faster?
                // TODO: is it ok to use "?" inserted for lonely hash as user name?
                QString user = m_hashesTable->data(m_hashesTable->index(i, 0)).toString();
                QString hash = m_hashesTable->data(m_hashesTable->index(i, 2)).toString();
                // TODO: is it ok to use \n?
                temp << QString("%1:%2::%3\n").arg(user).arg(hash).arg(hash);
            }
            m_temp->close();
        } else {
            // TODO: Error: could not open temp file.
        }
    }

    // TODO: Are these signals called always in order
    //       started-finished? Or is it unpredictable?
    // When John starts we enable stop button.
    // TODO: Is it ok if user clicks between his previous click and
    //       button disables?
    // TODO: Should we disable/enable status button?
    m_ui->actionPause_Attack->setEnabled(true);
    // When John starts we start capturing passwords.
    // TODO: Currently we set timer to 10 minutes. Make it
    //       customizable.
    // TODO: What about adoptive time intervals? Something like 1, 1,
    //       2, 2, 2, 2, 5, 5, 5, 5, 10, 10, 10, 10, 10, ...
    // TODO: John updates pot in time listed in configuration file.
    //       So it is needed to force John update file using sighup or
    //       similar way or to change configuration file.
    //       Also it is possible to implement terminal emulator to
    //       take passwords from stdout without buffering.
    //       It relates with picking status of John.
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
    // TODO: Do all platforms supported by Qt have 4 or bytes in int?
    // TODO: Better maximum value for spin box? Conceptually other way?
    m_showTimer.start(m_timeIntervalPickCracked * 1000);
    // If we continue cracking than there could already be cracked
    // passwords so we check status.
    callJohnShow();
}

void MainWindow::showJohnError(QProcess::ProcessError error)
{
    QString message;
    // TODO: define is a bad style for c++.
    // NOTE: space before :: is necessary.
#define C(code, text) case QProcess :: code: message = tr(text); break;
#define P(code) C(code, "Problem with john: " # code);
    switch (error) {
        C(FailedToStart,
          "John failed to start. "
          "Check your Path to John setting. "
          "Check permissions on respective file.");
        C(Crashed, "John crashed.");
        P(Timedout);
        P(WriteError);
        P(ReadError);
        P(UnknownError);
    default:
        message = tr("There is a problem. Johnny could not handle it.");
    }
#undef P
#undef C
    QMessageBox::critical(
        this,
        tr("Johnny"),
        // TODO: show path to john in error messages.
        message);
}

void MainWindow::showJohnFinished()
{
    // TODO: Should we place a message about it into output buffer?
    // When John finishes we enable start button and disable stop
    // button.
    m_ui->actionPause_Attack->setEnabled(false);
    m_ui->actionStart_Attack->setEnabled(true);
    m_ui->actionOpen_Password->setEnabled(true);
    checkNToggleActionsLastSession();
    // When John stops we need to stop timer and to look status last
    // time.
    // TODO: currently if john crashed we do not call john.
    m_showTimer.stop();
    callJohnShow();
}

void MainWindow::callJohnShow()
{
    // TODO: if john returns immediately then we call it again before
    //       it finishes. No good solution. Only workaround.
    m_showJohnProcess.waitForFinished(1000);
    if (m_showJohnProcess.state() != QProcess::NotRunning)
        m_showJohnProcess.kill();
    m_showJohnProcess.waitForFinished(1000);

    QStringList parameters;
    // We add current format key if it is not empty.
    if (m_format != "")
        parameters << m_format;
    parameters << "--show" << m_temp->fileName();
    m_showJohnProcess.start(m_pathToJohn, parameters);
}

void MainWindow::readJohnShow()
{
    // TODO: Read John's output while it runs. Do not wait before it
    //       finishes. For 0.5M of password this code should not work.
    //       Even if it works it seems to be ineffective for big
    //       files.
    // We read all output.
    QByteArray output = m_showJohnProcess.readAllStandardOutput();
    QTextStream outputStream(output);
    // We parse it.
    // We read output line by line and take user name and password.
    // Then we find a row with such user and insert password there.
    // TODO: This is ineffective implementation. May QVector::index()
    //       be god for search in model internals?
    // TODO: We could have more than 2 fields. It seems to be useful
    //       to look on them too.
    // TODO: What if there are 2 or more rows with 1 user name?
    QString line;
    line = outputStream.readLine();
    // If john did not yet cracked anything then john does not emit
    // empty line before statistics.
    QString firstLine;
    firstLine = line;
    // We read to the end or before empty line.
    while (!line.isNull() && line != "") {
        // TODO: could not it be done faster?
        line.remove(QRegExp("\\r?\\n"));
        // We split lines to fields.
        // TODO: What if password contains semicolon?
        // TODO: What if password contains new line?
        int left = line.indexOf(":");
        int right = line.lastIndexOf("::");
        // TODO: check we found left and right and left is not right.
        QString password = line.mid(left + 1, right - left - 1);
        QString hash = line.mid(right + 2);
        // We handle password.
        // If we found user then we put password in table.
        // TODO: What if there two rows with one user name?
        // TODO: What if we did not have 2 fields? Could
        //       John's output be wrong?
        // TODO: What if we do not find row? Note user. Take into
        //       account that we remove value after use.
        // TODO: We overwrite values each time.
        foreach (int row, m_tableMap.values(hash)) {
            // TODO: claim if overwrite with other value. Be aware of
            //       lm with its ???????HALF2.
            m_hashesTable->setData(
                m_hashesTable->index(row, 1),
                password);
        }
        // We remove value to speed up.
        m_tableMap.remove(hash);
        // We continue reading with next line.
        line = outputStream.readLine();
    }
    QString lastLine;
    if (!line.isNull()) {
        // We are on the last line.
        // TODO: Really? We are after empty line.
        // We take counts of cracked passwords and of left hashes.
        // We read count of cracked password hashes.
        // TODO: Could we read after end?
        lastLine = outputStream.readLine();
    } else {
        lastLine = firstLine;
    }
    // TODO: Is following regexp always right?
    QRegExp crackedLeft("(\\d+)\\D+(\\d+)");
    int pos = crackedLeft.indexIn(lastLine);
    if (pos > -1) {
        // TODO: check toInt success.
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
            // TODO: May it be better to show entire string from John?
            //       Translation?
            m_ui->progressBar->setRange(0, crackedCount + leftCount);
            m_ui->progressBar->setValue(crackedCount);
            // TODO: Is not such format too complex?
            // TODO: May it be better to not change format during run?
            // TODO: When attack starts progress bar goes left to
            //       right and back before we set new format up.
            // TODO: Format is shown as key. Enough good?
            //       Brackets are shown always.
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
//       on_pushButton_ApplySettings_clicked,
//       on_pushButton_ApplySaveSettings_clicked,
//       on_pushButton_ResetSettings_clicked.
//       And of course you should put elements on the form.
//       And for each setting there should method for auto
//       application.
// TODO: It seems to be ugly. Refactoring is needed.

void MainWindow::warnAboutDefaultPathToJohn()
{
    // TODO: Handle empty path specifically.
    // TODO: On startup this message is shown before main window. Bad?
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
    // Find john on PATH
    QString john;
    QStringList possiblePaths;
    // TODO: list of names for john.
    // TODO: list of common names for john (i.e. john-gpu).
    // TODO: hint for user that he could use just 'john' for the
    //       setting but it has its specific pros and cons.
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    // TODO: Windows portability (semicolon instead of colon as delimiter).
    // TODO: could there be escaped delimiter?
    // TODO: Translated path to john? Any better solution for package
    //       maintainers in distributions?
    // TODO: it is bad implementation of search on PATH. Are not there
    //       predefined better one?
    QString johnName = tr("john");
    foreach (QString dir, env.value("PATH").split(":")) {
        possiblePaths << QDir(dir).filePath(johnName);
    }
    // Predefined defaults
    // TODO: we should not check paths default for linux on windows. On cygwin?
    possiblePaths << "/usr/sbin/john";
    // Find first readable, executable file from possible
    foreach (QString path, possiblePaths) {
        QFileInfo iJohn(path);
        if (iJohn.isReadable() && iJohn.isExecutable()) {
            john = path;
            break;
        }
    }

    // We have hard coded default settings in here.
    // TODO: Any better solution?
    // We just write all our values to elements on the form.
    m_ui->comboBox_PathToJohn->setEditText(john);
    m_ui->spinBox_TimeIntervalPickCracked->setValue(10 * 60);
    m_ui->checkBox_AutoApplySettings->setChecked(false);
}

void MainWindow::on_pushButton_FillSettingsWithDefaults_clicked()
{
    fillSettingsWithDefaults();
    warnAboutDefaultPathToJohn();
}

void MainWindow::on_pushButton_BrowsePathToJohn_clicked()
{
    // We pop a dialog to choose a file to open.
    // TODO: Copy-pasting is evil! (on_pushButton_WordlistFileBrowse_clicked)
    // TODO: What happens when John writes something while dialog
    //       opened?
    // TODO: Move dialog creation and setting up into window constructor.
    // TODO: Should we save this dialog on form to make it remember
    //       last path?
    // TODO: Yet another "browse" button... Do not you want to make
    //       without copying of code?
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFile);
    // TODO: Dialog could allow user to select multiple files. May it
    //       be good to support this ability? To concatenate selected file?
    if (dialog.exec()) {
        QString fileName = dialog.selectedFiles()[0];
        // We put file name into field for it.
        // TODO: File name does not appear in history (drop down list).
        m_ui->comboBox_PathToJohn->setEditText(fileName);
    }
}

void MainWindow::on_pushButton_ApplySettings_clicked()
{
    // We copy settings from elements on the form to the settings
    // object with current settings.
    m_pathToJohn = m_ui->comboBox_PathToJohn->currentText();
    m_timeIntervalPickCracked = m_ui->spinBox_TimeIntervalPickCracked->value();
    m_autoApplySettings = m_ui->checkBox_AutoApplySettings->isChecked();
}

void MainWindow::on_pushButton_ApplySaveSettings_clicked()
{
    // We apply settings first.
    // TODO: It is not a good design that we call button's handler
    //       that is really do something useful.
    on_pushButton_ApplySettings_clicked();
    // We store settings.
    m_settings.setValue("PathToJohn", m_ui->comboBox_PathToJohn->currentText());
    m_settings.setValue("TimeIntervalPickCracked", m_ui->spinBox_TimeIntervalPickCracked->value());
    m_settings.setValue("AutoApplySettings", m_ui->checkBox_AutoApplySettings->isChecked());
}

void MainWindow::on_pushButton_ResetSettings_clicked()
{
    // We copy settings from stored settings object to our current
    // settings points.
    // Really we copy stored settings to the form and then apply
    // settings.
    // TODO: claim on empty fields. Probably on all together.
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
    // We apply settings.
    // TODO: Again... Button's handler do useful work but named
    //       inappropriately because it is handler.
    on_pushButton_ApplySettings_clicked();
}

// Handlers for settings auto application

void MainWindow::on_comboBox_PathToJohn_editTextChanged()
{
    // If auto application is turned on then we apply settings.
    // TODO: Should we apply only one settings or all?
    //       Currently we apply all settings (copy the same values).
    //       Maybe it would be better to postpone settings application
    //       to the moment when settings are really needed. Lazy
    //       application.
    if (m_autoApplySettings)
        on_pushButton_ApplySettings_clicked();
}

void MainWindow::on_spinBox_TimeIntervalPickCracked_valueChanged()
{
    // TODO: Copy-pasting is evil!
    //       (on_comboBox_PathToJohn_valueChanged)
    if (m_autoApplySettings)
        on_pushButton_ApplySettings_clicked();
}

void MainWindow::on_checkBox_AutoApplySettings_stateChanged()
{
    // First goal is to disable 'apply' button and to apply settings
    // when auto application is turned on.
    // TODO: At program start we might need to do it too. Check!
    bool autoApply = m_ui->checkBox_AutoApplySettings->isChecked();
    m_ui->pushButton_ApplySettings->setEnabled(!autoApply);
    if (autoApply)
        on_pushButton_ApplySettings_clicked();
    // Second goal is auto application for auto application setting itself.
    // NOTE: Deactivation of auto application will be auto applied.
    // NOTE: Auto application is a setting too. At least it would be
    //       good to remember its state between program runs.
    // TODO: Copy-pasting is evil!
    //       (on_comboBox_PathToJohn_valueChanged)
    if (m_autoApplySettings)
        on_pushButton_ApplySettings_clicked();
}

// Statistics page code

void MainWindow::on_pushButton_StatisticsUpdateStatus_clicked()
{
    // Working time
    // We could not just subtract one time from another. But we could
    // know days and seconds between two time points.
    //
    // We check whether John is running.
    if (m_johnProcess.state() == QProcess::Running) {
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
        // TODO: Other format?
        // TODO: String translation?
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
