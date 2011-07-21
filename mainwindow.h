/*
 * Copyright (c) 2011 Shinnok <raydenxy at gmail.com>.  See LICENSE.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QProcess>

#include "tablemodel.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_actionStart_Attack_triggered();
    void updateJohnOutput();
    void showJohnFinished();
    void showJohnStarted();

    void on_actionPause_Attack_triggered();

    void on_pushButton_JohnStatus_clicked();

    void selectPage();

private:
    Ui::MainWindow *m_ui;
    TableModel *m_hashmodel;
    QProcess m_johnProcess;
};

#endif // MAINWINDOW_H
