/*
 * Copyright (c) 2011 Shinnok <raydenxy at gmail.com>.  See LICENSE.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>

#include "tablemodel.h"
#include "johnThread.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

private slots:
    void on_pushButton_clicked();
    void on_actionStart_Attack_triggered();
    void updateJohnOutput(const QString session,
                          QByteArray stdout,
                          QByteArray stderr);

    void on_actionPause_Attack_triggered();

    void on_pushButton_JohnStatus_clicked();

    void selectPage();

private:
    Ui::MainWindow *ui;
    TableModel *hashmodel;
    JohnThread *th;

signals:
    void killJohn();
    void johnStatus();
};

#endif // MAINWINDOW_H
