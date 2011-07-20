/*
 * Copyright (c) 2011 Shinnok <raydenxy at gmail.com>.  See LICENSE.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QToolButton>
#include <QStringListModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), hashmodel(NULL)
{
    ui->setupUi(this);

    connect(ui->listWidgetTabs, SIGNAL(itemSelectionChanged()),
            this, SLOT(selectPage()));

    ui->listWidgetTabs->setMaximumWidth(ui->listWidgetTabs->sizeHintForColumn(0) + 2);

    // TODO: How to select Passwords tab through mainwindow.ui?
    //       Property 'selected' with 'bool' value 'true' did not work for me.
    // We select first item/tab on list.
    ui->listWidgetTabs->setCurrentRow(0);

    // TODO: Is it possible to make it through mainwindow.ui?
    // TODO: Does this find condition works for all strings?
    foreach (QListWidgetItem *item, ui->listWidgetTabs->findItems("", Qt::MatchContains))
        item->setSizeHint(QSize(ui->listWidgetTabs->sizeHintForColumn(0), 60));

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
    sessionMenu->addAction(ui->actionNew_Session);
    sessionMenu->addAction(ui->actionSave_Session);
    // We create a button.
    QToolButton *sessionMenuButton = new QToolButton(this);
    // We set default action and menu for the button.
    sessionMenuButton->setDefaultAction(ui->actionOpen_Session);
    sessionMenuButton->setMenu(sessionMenu);
    // We set button up to have desired look and behaviour.
    sessionMenuButton->setPopupMode(QToolButton::MenuButtonPopup);
    // TODO: May it be better to derive this setting from the toolbar?
    sessionMenuButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    // We put the button onto the toolbar.
    ui->mainToolBar->insertWidget(ui->actionOpen_Password, sessionMenuButton);

    // TODO: Could we make connections easier?
    // We connect John process' signals with our slots.
    // John was ended.
    // TODO
    // John was started.
    // TODO
    // John wrote something.
    connect(&johnProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(updateJohnOutput()));
    connect(&johnProcess, SIGNAL(readyReadStandardError()),
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
    delete ui;
}

void MainWindow::selectPage()
{
    ui->stackedWidget->setCurrentIndex(ui->listWidgetTabs->currentRow());
}

void MainWindow::on_pushButton_clicked()
{
    if (hashmodel != NULL) {
        delete hashmodel;
        hashmodel = NULL;
    }

    hashmodel = new TableModel(this);

    ui->tableView_Hashes->setModel(hashmodel);

    for (int i = 0; i < TABLE_ROWS; i++) {
        hashmodel->setData(hashmodel->index(i, 0), QString("Rick%1").arg(i));
        hashmodel->setData(hashmodel->index(i, 1), QString("6817f89c171a439b3d0418a18a236001"));
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
    johnProcess.start("/usr/sbin/john", parameters);
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
    ui->plainTextEdit_JohnOut->insertPlainText(johnProcess.readAllStandardOutput()); // read output buffer
    ui->plainTextEdit_JohnOut->insertPlainText(johnProcess.readAllStandardError()); // read error buffer
}

void MainWindow::on_actionPause_Attack_triggered()
{
    // We ask John to exit.
    // TODO: Is it ok to call it even if process is not running?
    // TODO: Do not we need to call kill instead if John is too busy?
    // NOTE: We could leave it for user: we count button presses and for
    //       the first time we call terminate and for next times we
    //       call kill.
    johnProcess.terminate();
}

void MainWindow::on_pushButton_JohnStatus_clicked()
{
    // When we want to get John status we send enter to John. Then
    // John write something to its stdout. We do not need to read its
    // output here because when output is ready to be read a signal is
    // fired and we read John output with status as any other John's
    // output.
    // TODO: However it does not work as of we do not have terminal.
    johnProcess.write("a\r\n");
}
