/*
 * Copyright (c) 2011 Shinnok <raydenxy at gmail.com>.  See LICENSE.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QProcess>
#include <QAbstractTableModel>
#include <QString>
#include <QTimer>
#include <QByteArray>
#include <QTextStream>

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
    void on_actionPause_Attack_triggered();
    void on_actionOpen_Password_triggered();
    void on_pushButton_JohnStatus_clicked();

    void updateJohnOutput();
    void showJohnFinished();
    void showJohnStarted();
    void selectPage();
    void replaceTableModel(QAbstractTableModel *newTableModel);

    void callJohnShow();
    void readJohnShow();

private:
    Ui::MainWindow *m_ui;
    QAbstractTableModel *m_hashsTable;
    // TODO: Probably the right place for this field is in table model.
    //       But this needs to have abstract interface that supports
    //       connection with files. Someone could suppose to drop
    //       generated tables out but if we want to have tables
    //       joining, editing and so on tables that is not connected
    //       with files yet are necessary.
    //       However now this is here.
    QString m_hashsFileName;
    QProcess m_johnProcess;
    // To catch cracked passwords we use timer and john --show.
    QTimer m_showTimer;
    QProcess m_showJohnProcess;
};

#endif // MAINWINDOW_H
