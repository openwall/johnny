/*
 * Copyright (c) 2011 Shinnok <raydenxy at gmail.com>.  See LICENSE.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QToolButton>
#include <QStringListModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_ui(new Ui::MainWindow), m_hashmodel(NULL)
{
    m_ui->setupUi(this);

    connect(m_ui->listWidgetTabs, SIGNAL(itemSelectionChanged()),
            this, SLOT(selectPage()));

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

void MainWindow::selectPage()
{
    m_ui->stackedWidget->setCurrentIndex(m_ui->listWidgetTabs->currentRow());
}

void MainWindow::on_pushButton_clicked()
{
    if (m_hashmodel != NULL) {
        delete m_hashmodel;
        m_hashmodel = NULL;
    }

    m_hashmodel = new TableModel(this);

    m_ui->tableView_Hashes->setModel(m_hashmodel);

    for (int i = 0; i < TABLE_ROWS; i++) {
        m_hashmodel->setData(m_hashmodel->index(i, 0), QString("Rick%1").arg(i));
        m_hashmodel->setData(m_hashmodel->index(i, 1), QString("6817f89c171a439b3d0418a18a236001"));
    }
}

void MainWindow::on_actionStart_Attack_triggered()
{
    QStringList parameters;

    parameters << "--restore"; // no params for now

    // To start John we have predefined process object. That object's
    // signals are already connected with our slots. So we need only
    // start it.
    //
    // We start John.
    m_johnProcess.start("/usr/sbin/john", parameters);
}

void MainWindow::updateJohnOutput()
{
    // TODO: There was a session string passed here. Hence the
    //       question what is right: one window could have multiple sessions or at one
    //       time there could be only one sesson opened?
    // NOTE: If there could be only one session in/per window then it
    //       is possible to have session name here through window's field.
    // TODO: Session name should be displayed.
    //ui->plainTextEdit_JohnOut->insertPlainText("Session file: " + session + "\n");
    m_ui->plainTextEdit_JohnOut->insertPlainText(m_johnProcess.readAllStandardOutput()); // read output buffer
    m_ui->plainTextEdit_JohnOut->insertPlainText(m_johnProcess.readAllStandardError()); // read error buffer
}

void MainWindow::on_actionPause_Attack_triggered()
{
    // We ask John to exit.
    // TODO: Is it ok to call it even if process is not running?
    // TODO: Do not we need to call kill instead if John is too busy?
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
}

void MainWindow::showJohnFinished()
{
    // TODO: Should we place a message about it into output buffer?
    // When John finishes we enable start button and disable stop
    // button.
    m_ui->actionPause_Attack->setEnabled(false);
    m_ui->actionStart_Attack->setEnabled(true);
}
