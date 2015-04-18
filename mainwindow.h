/*
 * Copyright (c) 2011 Shinnok <raydenxy at gmail.com>.
 * Copyright © 2011,2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>.  See LICENSE.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QProcess>
#include <QAbstractTableModel>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QByteArray>
#include <QTextStream>
#include <QSettings>
#include <QDateTime>
#include <QMultiMap>
#include <QTemporaryFile>
#include <QProcess>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void closeEvent(QCloseEvent *event);
    ~MainWindow();

private slots:
    /* void on_pushButton_clicked(); */
    void on_actionStart_Attack_triggered();
    void on_actionResume_Attack_triggered();
    void on_actionPause_Attack_triggered();
    void on_actionCopyToClipboard_triggered();
    void on_actionOpen_Password_triggered();
    void on_actionOpen_Last_Session_triggered();
    /* void on_pushButton_JohnStatus_clicked(); */
    void on_listWidgetTabs_itemSelectionChanged();
    void on_pushButton_WordlistFileBrowse_clicked();
    void on_pushButton_FillSettingsWithDefaults_clicked();
    void on_pushButton_BrowsePathToJohn_clicked();
    void on_pushButton_ApplySettings_clicked();
    void on_pushButton_ApplySaveSettings_clicked();
    void on_pushButton_ResetSettings_clicked();
    void on_comboBox_PathToJohn_editTextChanged();
    void on_spinBox_TimeIntervalPickCracked_valueChanged();
    void on_checkBox_AutoApplySettings_stateChanged();
    void on_pushButton_StatisticsUpdateStatus_clicked();

    void updateJohnOutput();
    void showJohnFinished();
    void showJohnStarted();
    void showJohnError(QProcess::ProcessError error);
    void replaceTableModel(QAbstractTableModel *newTableModel);

    void startJohn(QStringList params);

    void callJohnShow();
    void readJohnShow();

    void fillSettingsWithDefaults();
    void warnAboutDefaultPathToJohn();

    void checkNToggleActionsLastSession();

    bool readPasswdFile(const QString &fileName);

    bool checkSettings();

private:
    Ui::MainWindow *m_ui;
    QAbstractTableModel *m_hashesTable;
    // TODO: Probably the right place for this field is in table model.
    //       But this needs to have abstract interface that supports
    //       connection with files. Someone could suppose to drop
    //       generated tables out but if we want to have tables
    //       joining, editing and so on tables that is not connected
    //       with files yet are necessary.
    //       However now this is here.
    QString m_hashesFileName;
    QString m_session;
    QProcess m_johnProcess;
    // To catch cracked passwords we use timer and john --show.
    QTimer m_showTimer;
    QProcess m_showJohnProcess;
    // Format key to use with --show.
    // With this key current John was started.
    QString m_format;
    // Current application settings
    // Modified settings are stored on the form, this settings
    // is used during this instance of application work. Stored
    // settings are stored on the disk and will be loaded next time
    // application start.
    // TODO: Group settings into separate class with support for
    //       saving and so on.
    // Path to John's binary
    QString m_pathToJohn;
    // Interval between loading of cracked passwords
    int m_timeIntervalPickCracked;
    // Should we use modified settings right after modification? Or
    // should we wait user to click 'apply' button.
    bool m_autoApplySettings;
    // Stored settings
    QSettings m_settings;
    // Date and time of the start of the sttack
    QDateTime m_startDateTime;
    // Map (hash table) for fast access after `john --show`
    QMultiMap<QString, int> m_tableMap;
    // Holder for temporary file for `john --show`
    QTemporaryFile *m_temp;
};

#endif // MAINWINDOW_H
