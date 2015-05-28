/*
 * Copyright (c) 2011 Shinnok <admin at shinnok.com>.
 * Copyright (c) 2011, 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>.
 * See LICENSE for details.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "johnprocess.h"

#include <QMainWindow>
#include <QListWidgetItem>
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
#include <QPlainTextEdit>


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QSettings& settings);
    ~MainWindow();

private slots:
    // UI actions
    void startAttack();
    void resumeAttack();
    void pauseAttack();
    void actionCopyToClipboardTriggered();
    void openPasswordFile();
    void openLastSession();
    void listWidgetTabsSelectionChanged();
    void buttonWordlistFileBrowseClicked();
    void buttonFillSettingsWithDefaultsClicked();
    void buttonBrowsePathToJohnClicked();
    void checkBoxAutoApplySettingsStateChanged();
    void updateStatistics();
    void settingsChangedByUser();

    // JtR backend
    void updateJohnOutput();
    void showJohnFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void showJohnStarted();
    void showJohnError(QProcess::ProcessError error);
    void replaceTableModel(QAbstractTableModel *newTableModel);
    void startJohn(QStringList params);
    void callJohnShow();
    void readJohnShow();

    // Settings related
    void fillSettingsWithDefaults();
    void restoreLastSavedSettings();
    void applySettings();
    void applyAndSaveSettings();
    void warnAboutDefaultPathToJohn();
    void verifySessionState();
    bool readPasswdFiles(const QStringList &fileNames);
    bool checkSettings();

    // Helpers
    void appendLog(const QString& text);
    QStringList getAttackParameters();

protected:
    void closeEvent(QCloseEvent *event);
    // For the OS X QProgressBar issue
    // https://github.com/shinnok/johnny/issues/11
    bool eventFilter(QObject *watched, QEvent *event);

private:
    bool                 m_terminate;
    Ui::MainWindow      *m_ui;
    QAbstractTableModel *m_hashesTable;
    QString              m_appDataPath;

    QStringList     m_hashesFilesNames;
    QString         m_session;
    JohnProcess     m_johnProcess;
    // Date and time of the start of the sttack
    QDateTime m_startDateTime;

    // To read cracked passwords we use this timer and john --show.
    QTimer      m_showTimer;
    QProcess    m_showJohnProcess;
    // Format key to use with --show.
    // With this key current John was started.
    QString     m_format;
    // Holder for temporary file for `john --show`
    QTemporaryFile *m_temp;
    // Map (hash table) for fast access after `john --show`
    QMultiMap<QString, int> m_tableMap;

    // Current application settings
    // Modified settings are stored on the form, this settings
    // is used during this instance of application work. Stored
    // settings are stored on the disk and will be loaded next time
    // application start.

    QSettings&  m_settings;
    QString     m_pathToJohn;
    // Interval between loading of cracked passwords
    int         m_timeIntervalPickCracked;
    // Should we use modified settings right after modification? Or
    // should we wait user to click 'apply' button.
    bool        m_autoApplySettings;
};

#endif // MAINWINDOW_H
