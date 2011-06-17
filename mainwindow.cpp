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
