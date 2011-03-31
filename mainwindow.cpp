#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QToolButton>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

}

MainWindow::~MainWindow()
{
    delete ui;
}
