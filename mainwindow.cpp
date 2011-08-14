/*
 * Copyright (c) 2011 Shinnok <raydenxy at gmail.com>.  See LICENSE.
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_ui(new Ui::MainWindow), m_hashesTable(NULL)
{
    m_ui->setupUi(this);

    m_ui->listWidgetTabs->setMaximumWidth(m_ui->listWidgetTabs->sizeHintForColumn(0) + 2);

    // TODO: How to select Passwords tab through mainwindow.ui?
    //       Property 'selected' with 'bool' value 'true' did not work for me.
    // We select first item/tab on list.
    m_ui->listWidgetTabs->setCurrentRow(0);

    // TODO: Is it possible to make it through mainwindow.ui?
    // TODO: Does this find condition works for all strings?
    foreach (QListWidgetItem *item, m_ui->listWidgetTabs->findItems("", Qt::MatchContains))
        item->setSizeHint(QSize(m_ui->listWidgetTabs->sizeHintForColumn(0), 60));

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

    // TODO: Could we make connections easier?
    // We connect John process' signals with our slots.
    // John was ended.
    // TODO: It will good to show exit status and exit code to user.
    connect(&m_johnProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(showJohnFinished()));
    // John was started.
    connect(&m_johnProcess, SIGNAL(started()),
            this, SLOT(showJohnStarted()));
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
    connect(m_ui->radioButton_WordlistMode, SIGNAL(toggled(bool)),
            m_ui->checkBox_WordlistModeRulesName, SLOT(setEnabled(bool)));
    connect(m_ui->radioButton_WordlistMode, SIGNAL(toggled(bool)),
            m_ui->comboBox_WordlistModeRulesName, SLOT(setEnabled(bool)));
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

//    TableModel *passmodel = new TableModel();

//    ui->tableView_Passwords->setModel(passmodel);

//    for (int i = 0; i < TABLE_ROWS; i++) {
//        passmodel->setData(passmodel->index(i, 0), QString("Rick%1").arg(i));
//        passmodel->setData(passmodel->index(i, 1), QString("Never gonna give you up!"));
//    }

}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::on_pushButton_WordlistFileBrowse_clicked()
{
    // We pops a dialog to choose a file to open.
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

void MainWindow::on_comboBox_Format_currentIndexChanged(const QString& text)
{
    // When selected format changed to "md5-gen" we enable subformat
    // list. In other cases it should be disabled.
    bool enabled = text == "md5-gen";
    m_ui->label_Subformat->setEnabled(enabled);
    m_ui->comboBox_Subformat->setEnabled(enabled);
}

void MainWindow::on_listWidgetTabs_itemSelectionChanged()
{
    m_ui->stackedWidget->setCurrentIndex(m_ui->listWidgetTabs->currentRow());
}

void MainWindow::replaceTableModel(QAbstractTableModel *newTableModel)
{
    // TODO: Check argument.

    // We delete existing model if any.
    if (m_hashesTable != NULL) {
        delete m_hashesTable;
        m_hashesTable = NULL;
    }
    // We remember new model.
    m_hashesTable = newTableModel;
    // We connect table view with new model.
    m_ui->tableView_Hashes->setModel(newTableModel);
}

void MainWindow::on_pushButton_clicked()
{
    replaceTableModel(new TableModel(this));
    // We reset file name because this model is generated and does not
    // have connected file.
    m_hashesFileName = "";

    for (int i = 0; i < TABLE_ROWS; i++) {
        m_hashesTable->setData(m_hashesTable->index(i, 0), QString("Rick%1").arg(i));
        m_hashesTable->setData(m_hashesTable->index(i, 1), QString("6817f89c171a439b3d0418a18a236001"));
    }
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

    // We pops a dialog to choose a file to open.
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
        // We replace existing model with new one.
        replaceTableModel(new FileTableModel(fileName, this));
        // After new model remembered we remember its file name.
        m_hashesFileName = fileName;
    }
}

void MainWindow::on_actionStart_Attack_triggered()
{
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
        // TODO: What is better format to use for keys: -key: or
        //       --key= or something else?
        // We remember format key to be used with '-show' to take
        // progress.
        // TODO: Instead of remembering of keys we could lock options.
        //       What is better?
        m_format = ("-format:" + m_ui->comboBox_Format->currentText());
        parameters << m_format;
    } else {
        // In case we do not have explicit format we erase remembered
        // key.
        m_format = "";
    }
    // Subformat
    // TODO: We put logic to determine either we need subformat
    //       option into enable/disable switcher. Bad design.
    if (m_ui->comboBox_Subformat->isEnabled()) {
        // We need to cut subformat from string because strings in this
        // list contain comments/descriptions.
        // Strings could be like:
        // md5_gen(0): md5($p)  (raw-md5) 
        // md5_gen(1001) md5(md5(md5(md5($p))))
        // User formats does not have semicolon between format name
        // and description. So we will restrict our pattern with right
        // brace.
        //
        // We copy string, truncate it to end with right brace.
        QString subformat = m_ui->comboBox_Subformat->currentText();
        subformat.truncate(subformat.indexOf(")") + 1);
        // We remember subformat to use later with '-show' to take
        // progress.
        m_subformat = "-subformat:" + subformat;
        parameters << m_subformat;
    } else {
        // If we do not use subformat key we remember it.
        m_subformat = "";
    }
    // Modes
    if (m_ui->radioButton_DefaultBehaviour->isChecked()) {
        // Default behaviour - no modes
        // There are no options here.
    } else if (m_ui->radioButton_SingleCrackMode->isChecked()) {
        // "Single crack" mode
        parameters << "-single";
        // External mode, filter
        // TODO: It is applicable for 3 formats. Copy-pasting is evil!
        // TODO: Warn if checkbox is checked and there is not text in
        //       combobox. For other empty fields it would great to
        //       warn too.
        if (m_ui->checkBox_SingleCrackModeExternalName->isChecked())
            parameters << ("-external:" + m_ui->comboBox_SingleCrackModeExternalName->currentText());
    } else if (m_ui->radioButton_WordlistMode->isChecked()) {
        // Wordlist mode
        parameters << ("-wordlist:" + m_ui->comboBox_WordlistFile->currentText());
        // Rules
        // They could appear with or without name.
        // TODO: It would be great to disable name selection before
        //       rules are chosen. It is opposite design to subformat list.
        if (m_ui->checkBox_WordlistModeRules->isChecked()) {
            // If rules are selected then we distinguish either name
            // is needed two.
            if (m_ui->checkBox_WordlistModeRulesName->isChecked()) {
                // If name for rules is selected to be then we take it.
                // NOTE: Not all versions support this.
                // TODO: It would be great to notice user about this.
                // TODO: It would be great to notice user on any
                //       fails, not only here.
                // TODO: Calls to John makes interface to stick a bit.
                //       It is actual with often -show calls.
                parameters << ("-rules:" + m_ui->comboBox_WordlistModeRulesName->currentText());
            } else {
                // If no name is needed then we use just rules,
                // without name.
                parameters << "-rules";
            }
        }
        // External mode, filter
        if (m_ui->checkBox_WordlistModeExternalName->isChecked())
            parameters << ("-external:" + m_ui->comboBox_WordlistModeExternalName->currentText());
    } else if (m_ui->radioButton_IncrementalMode->isChecked()) {
        // "Incremental" mode
        // It could be with or without name.
        if (m_ui->checkBox_IncrementalModeName->isChecked()) {
            // With name
            parameters << ("-incremental:" + m_ui->comboBox_IncrementalModeName->currentText());
        } else {
            // Without name
            parameters << "-incremental";
        }
        // External mode, filter
        if (m_ui->checkBox_IncrementalModeExternalName->isChecked())
            parameters << ("-external:" + m_ui->comboBox_IncrementalModeExternalName->currentText());
    } else if (m_ui->radioButton_ExternalMode->isChecked()) {
        // External mode
        parameters << ("-external:" + m_ui->comboBox_ExternalModeName->currentText());
    }

    // We check that we have file name.
    if (m_hashesFileName != "") {
        // If file name is not empty then we have file, pass it to
        // John.
        // We add file name onto parameters list.
        parameters << m_hashesFileName;
        // To start John we have predefined process object. That object's
        // signals are already connected with our slots. So we need only
        // start it.
        //
        // We start John.
        m_johnProcess.start("/usr/sbin/john", parameters);
    } else {
        // Else we do not have connected file name so we ask user to save
        // file.
        // TODO: Do something here.
    }
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
    // TODO: We need to call this before exit too. In the other case Qt
    //       will show warning about closing with running process.
    // NOTE: We could leave it for user: we count button presses and for
    //       the first time we call terminate and for next times we
    //       call kill.
    m_johnProcess.terminate();
}

void MainWindow::on_pushButton_JohnStatus_clicked()
{
    // When we want to get John status we send enter to John. Then
    // John write something to its stdout. We do not need to read its
    // output here because when output is ready to be read a signal is
    // fired and we read John output with status as any other John's
    // output.
    // TODO: However it does not work as of we do not have terminal.
    // TODO: Why do we write to John even when it is not running?
    m_johnProcess.write("a\r\n");
}

void MainWindow::showJohnStarted()
{
    // TODO: Are these signals called always in order
    //       started-finished? Or is it unpredictable?
    // When John starts we enable stop button and disable start
    // button.
    // TODO: Is it ok if user clicks between his previous click and
    //       button disables?
    // TODO: Should we disable/enable status button?
    m_ui->actionPause_Attack->setEnabled(true);
    m_ui->actionStart_Attack->setEnabled(false);
    // When John starts we start capturing passwords.
    // TODO: Currently we set timer to 10 seconds. Make it
    //       customizable.
    // TODO: What about adoptive time intervals? Something like 1, 1,
    //       2, 2, 2, 2, 5, 5, 5, 5, 10, 10, 10, 10, 10, ...
    m_showTimer.start(10000);
    // If we continue cracking than there could already be cracked
    // passwords so we check status.
    callJohnShow();
}

void MainWindow::showJohnFinished()
{
    // TODO: Should we place a message about it into output buffer?
    // When John finishes we enable start button and disable stop
    // button.
    m_ui->actionPause_Attack->setEnabled(false);
    m_ui->actionStart_Attack->setEnabled(true);
    // When John stops we need to stop timer and to look status last
    // time.
    m_showTimer.stop();
    callJohnShow();
}

void MainWindow::callJohnShow()
{
    QStringList parameters;
    // We add current format and subformat keys if they are not empty.
    if (m_format != "")
        parameters << m_format;
    if (m_subformat != "")
        parameters << m_subformat;
    parameters << "--show" << m_hashesFileName;
    // TODO: Customizable path to John.
    m_showJohnProcess.start("/usr/sbin/john", parameters);
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
    // We read to the end or before empty line.
    // TODO: In the end John says count of cracked password. Read it.
    while (!line.isNull() && line != "") {
        // We split lines to fields.
        // TODO: What if password contains semicolon?
        // TODO: What if password contains new line?
        QStringList fields = line.split(':');
        // We handle password.
        // We check all rows to have such user name.
        for (int i = 0; i < m_hashesTable->rowCount(); i++) {
            if (m_hashesTable->data(m_hashesTable->index(i, 0)) == fields.at(0)) {
                // If we found user then we put password in table.
                // TODO: What if there two rows with one user name?
                // TODO: What if we did not have 2 fields? Could
                //       John's output be wrong?
                // TODO: What if we did not find row? Note user.
                m_hashesTable->setData(m_hashesTable->index(i, 1), fields.at(1));
            }
        }
        // We continue reading with next line.
        line = outputStream.readLine();
    }
    // We are on the last line.
    // TODO: Really?
    // We take counts of cracked passwords and of left hashes.
    // We read count of cracked password hashes.
    // TODO: Could we read after end?
    // TODO: Is following sequence always right?
    int crackedCount;
    outputStream >> crackedCount;
    // We skip 3 words.
    QString skippedWord;
    outputStream >> skippedWord >> skippedWord >> skippedWord;
    // We read left count.
    int leftCount;
    outputStream >> leftCount;
    // We update progress bar.
    // TODO: May it be better to show entire string from John?
    //       Translation?
    m_ui->progressBar->setRange(0, crackedCount + leftCount);
    m_ui->progressBar->setValue(crackedCount);
    // TODO: Is not such format too complex?
    // TODO: May it be better to not change format during run?
    // TODO: When attack starts progress bar goes left to right and back before
    //       we set new format up.
    // TODO: Format and subformat are shown as keys. Enough good?
    //       Brackets are shown always.
    // TODO: This string is too long.
    m_ui->progressBar->setFormat(tr("%p% (%v/%m: %1 cracked, %2 left) [%3 %4]").arg(crackedCount).arg(leftCount).arg(m_format).arg(m_subformat));
}
