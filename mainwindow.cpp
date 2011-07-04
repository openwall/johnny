/*
 * Copyright (c) 2011 Shinnok <raydenxy at gmail.com>.  See LICENSE.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QToolButton>
#include <QStringListModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create listview items
    connect(ui->listWidgetTabs, SIGNAL(itemSelectionChanged()),
            this, SLOT(selectPage()));

    passwordsTab = new QListWidgetItem(ui->listWidgetTabs);
    passwordsTab->setIcon(QIcon(QString::fromUtf8(":/icons/tab_passwords.png")));
    passwordsTab->setText(tr("Passwords"));
    passwordsTab->setSelected(true);

    optionsTab = new QListWidgetItem(ui->listWidgetTabs);
    optionsTab->setIcon(QIcon(QString::fromUtf8(":/icons/tab_options.png")));
    optionsTab->setText(tr("Options"));

    statisticsTab = new QListWidgetItem(ui->listWidgetTabs);
    statisticsTab->setIcon(QIcon(QString::fromUtf8(":/icons/tab_statistics.png")));
    statisticsTab->setText(tr("Statistics"));

    benchmarkTab = new QListWidgetItem(ui->listWidgetTabs);
    benchmarkTab->setIcon(QIcon(QString::fromUtf8(":/icons/tab_benchmark.png")));
    benchmarkTab->setText(tr("Benchmark"));

    settingsTab = new QListWidgetItem(ui->listWidgetTabs);
    settingsTab->setIcon(QIcon(QString::fromUtf8(":/icons/tab_settings.png")));
    settingsTab->setText(tr("Settings"));

    outputTab = new QListWidgetItem(ui->listWidgetTabs);
    outputTab->setIcon(QIcon(QString::fromUtf8(":/icons/tab_output.png")));
    outputTab->setText(tr("Output"));

    logTab = new QListWidgetItem(ui->listWidgetTabs);
    logTab->setIcon(QIcon(QString::fromUtf8(":/icons/tab_log.png")));
    logTab->setText(tr("Log"));

    ui->listWidgetTabs->setMaximumWidth(ui->listWidgetTabs->sizeHintForColumn(0)+2);

    passwordsTab->setSizeHint(QSize(ui->listWidgetTabs->sizeHintForColumn(0), 60));
    optionsTab->setSizeHint(QSize(ui->listWidgetTabs->sizeHintForColumn(0), 60));
    statisticsTab->setSizeHint(QSize(ui->listWidgetTabs->sizeHintForColumn(0), 60));
    benchmarkTab->setSizeHint(QSize(ui->listWidgetTabs->sizeHintForColumn(0), 60));
    settingsTab->setSizeHint(QSize(ui->listWidgetTabs->sizeHintForColumn(0), 60));
    outputTab->setSizeHint(QSize(ui->listWidgetTabs->sizeHintForColumn(0), 60));
    logTab->setSizeHint(QSize(ui->listWidgetTabs->sizeHintForColumn(0), 60));

    this->hashmodel = NULL;

    QMenu *sessionMenu = new QMenu;

    //sessionMenu->addAction(ui->actionOpen_Session);
    sessionMenu->addAction(ui->actionNew_Session);
    sessionMenu->addAction(ui->actionSave_Session);
    ui->actionNew_Session->setIconVisibleInMenu(true);
    ui->actionSave_Session->setIconVisibleInMenu(true);


    QToolButton *sessionMenuButton = new QToolButton;

    sessionMenuButton->setMenu(sessionMenu);
    sessionMenuButton->setPopupMode(QToolButton::MenuButtonPopup);
    sessionMenuButton->setIcon(QIcon(":/icons/projectopen.png"));
    sessionMenuButton->setText(tr("Open Session"));
    sessionMenuButton->setToolTip(tr("Open a session file"));
    sessionMenuButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    ui->mainToolBar->insertWidget(ui->actionOpen_Password, sessionMenuButton);

//    TableModel *passmodel = new TableModel();

//    ui->tableView_Passwords->setModel(passmodel);

//    for(int i=0; i < TABLE_ROWS; i++){
//      passmodel->setData(passmodel->index(i, 0), QString("Rick%1").arg(i));
//      passmodel->setData(passmodel->index(i, 1), QString("Never gonna give you up!"));
//    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selectPage()
{
    if (passwordsTab->isSelected()) {
        ui->stackedWidget->setCurrentIndex(0);
    } else if (optionsTab->isSelected()) {
        ui->stackedWidget->setCurrentIndex(1);
    } else if (statisticsTab->isSelected()) {
        ui->stackedWidget->setCurrentIndex(2);
    } else if (benchmarkTab->isSelected()) {
        ui->stackedWidget->setCurrentIndex(3);
    } else if (settingsTab->isSelected()) {
        ui->stackedWidget->setCurrentIndex(4);
    } else if (outputTab->isSelected()) {
        ui->stackedWidget->setCurrentIndex(5);
    } else if (logTab->isSelected()) {
        ui->stackedWidget->setCurrentIndex(6);
    }
}

void MainWindow::on_pushButton_clicked()
{
    if(this->hashmodel != NULL){
        delete this->hashmodel;
        this->hashmodel = NULL;
    }

    hashmodel = new TableModel();

    ui->tableView_Hashes->setModel(hashmodel);

    for(int i=0; i < TABLE_ROWS; i++){
      hashmodel->setData(hashmodel->index(i, 0), QString("Rick%1").arg(i));
      hashmodel->setData(hashmodel->index(i, 1), QString("6817f89c171a439b3d0418a18a236001"));
    }
}

void MainWindow::on_actionStart_Attack_triggered()
{
    QStringList parameters;

    parameters << "--restore"; // no params for now

    QByteArray johnOut, johnErr;
    th = new JohnThread(johnOut, johnErr, parameters, this);

    th->start();
    connect(th, SIGNAL(johnOutput(QString,QByteArray,QByteArray)),
      this, SLOT(updateJohnOutput(const QString, QByteArray, QByteArray))); // nmapParser.cpp
}

void MainWindow::updateJohnOutput(const QString session, QByteArray stdout,
                                  QByteArray stderr)
{
    ui->plainTextEdit_JohnOut->insertPlainText("Session file: " + session + "\n");
    ui->plainTextEdit_JohnOut->insertPlainText(QString(stdout));
    ui->plainTextEdit_JohnOut->insertPlainText(QString(stderr));
}

void MainWindow::on_actionPause_Attack_triggered()
{
    emit killJohn();
}

void MainWindow::on_pushButton_JohnStatus_clicked()
{
    emit johnStatus();
}
