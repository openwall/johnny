/*
 * Copyright (c) 2011-2015 Shinnok <admin at shinnok.com>
 * Parts Copyright (c) 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>
 * Parts Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>
 * See LICENSE dist-file for details.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "hashsortfilterproxymodel.h"
#include "hashtypechecker.h"
#include "johnattack.h"
#include "johnprocess.h"
#include "johnsession.h"
#include "menu.h"
#include "openotherformatfiledialog.h"
#include "passwordfilemodel.h"

#include <QAbstractTableModel>
#include <QByteArray>
#include <QDateTime>
#include <QLabel>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QMultiMap>
#include <QPlainTextEdit>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QTemporaryFile>
#include <QTextStream>
#include <QTimer>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QSettings &settings);
    ~MainWindow();

private slots:
    // UI actions
    void startAttack();
    void resumeAttack();
    void pauseAttack();
    void actionCopyToClipboardTriggered();
    void actionOpenSessionTriggered(QAction *action);
    void openPasswordFile(QStringList fileNames = QStringList());
    void openLastSession();
    void tabsSelectionChanged(QAction *action);
    void buttonWordlistFileBrowseClicked();
    void buttonBrowsePathToJohnClicked();
    void updateStatistics();
    void restoreSessionOptions();
    void restoreDefaultAttackOptions(bool shouldClearFields = true);
    void checkForUpdates();
    void filterHashesTable();
    void showHashesTableContextMenu(const QPoint &pos);
    void showConsoleLogContextMenu(const QPoint &pos);
    void includeSelectedHashes();
    void excludeSelectedHashes();
    void setFilteringColumns();
    void resetFilters();
    void actionExportToTriggered(QAction *action);
    void exportTo(char delimiter, QString fileName);

    // JtR backend
    void updateJohnOutput();
    void showJohnFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void showJohnStarted();
    void showJohnError(QProcess::ProcessError error);
    void replaceTableModel(PasswordFileModel *newTableModel);
    void startJohn(QStringList args);
    void callJohnShow();
    void readJohnShow();
    void updateHashTypes(const QStringList &pathToPwdFile,
                         const QStringList &listOfTypesInFile,
                         const QStringList &detailedTypesPerRow);

    void guessPassword();
    void guessPasswordFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void setAvailabilityOfFeatures(bool isJumbo);
    void verifyJohnVersion();
    void getDefaultFormat();
    void getDefaultFormatFinished(int exitCode, QProcess::ExitStatus exitStatus);

    // Settings related
    void fillSettingsWithDefaults();
    void restoreSavedSettings();
    void applySettings();
    void applyAndSaveSettings();
    void warnAboutDefaultPathToJohn();
    bool readPasswdFiles(const QStringList &fileNames);
    bool checkSettings();
    void johnPathChanged();
    void invalidJohnPathDetected();
    bool validateJohnPath(QString path);

    // Helpers
    void appendLog(const QString &text);
    QStringList saveAttackParameters();

protected:
    void closeEvent(QCloseEvent *event);
    // For the OS X QProgressBar issue
    // https://github.com/shinnok/johnny/issues/11
    bool eventFilter(QObject *watched, QEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    bool                      m_terminate;
    Ui::MainWindow           *m_ui;
    Menu                     *m_sessionMenu;
    QMenu                    *m_hashTableContextMenu;
    QMenu                    *m_consoleLogContextMenu;
    PasswordFileModel        *m_hashTable;
    HashSortFilterProxyModel *m_hashTableProxy;

    JohnSession         m_sessionCurrent;
    QStringList         m_sessionHistory;
    QStringList         m_sessionPasswordFiles;
    JohnAttack          m_johnAttack;       // main JtR attack handler

    // To read cracked passwords we use this timer and john --show.
    QTimer                   m_showTimer;
    JohnHandler              m_johnShow;
    // Temporary file for `john --show` output
    QTemporaryFile          *m_johnShowTemp;
    // Map (hash table) for fast access after `john --show`
    QMultiMap<QString, int>  m_showTableMap;

    // Current application settings
    // Modified settings are stored on the form, this settings
    // is used during this instance of application work. Stored
    // settings are stored on the disk and will be loaded next time
    // application start.
    QSettings      &m_settings;
    QString         m_pathToJohn;
    // Interval between loading of cracked passwords
    int             m_timeIntervalPickCracked;

    JohnHandler     m_johnVersionCheck;
    bool            m_isJumbo;

    HashTypeChecker m_hashTypeChecker;
    JohnHandler     m_johnGuess;
    JohnAttack      m_johnDefaultFormat;
    bool            m_isDynamicFilteringEnabled;

    QWidget                     m_aboutWindow;
    QTimer                      m_labelSelectionHide;
    OpenOtherFormatFileDialog  *m_openOtherFormatDialog;

#ifdef Q_OS_OSX
    QLabel *m_progressStatsLabel;
#endif
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
    QLabel *m_filterDirectivesLabel;
#endif
};

#endif // MAINWINDOW_H
