/*
 * Copyright (c) 2011-2015 Shinnok <admin at shinnok.com>
 * Parts Copyright (c) 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>
 * Parts Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>
 * See LICENSE dist-file for details.
 */

#include "mainwindow.h"
#include "translator.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>

#ifdef Q_OS_WIN
#include <wincon.h>
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    SetConsoleCtrlHandler(NULL, TRUE);
#endif
    // We set application info up.
    // It is needed to be able to store settings easily.
    QCoreApplication::setOrganizationName("Openwall");
    QCoreApplication::setOrganizationDomain("openwall.com");
    QCoreApplication::setApplicationName("Johnny");

    QApplication app(argc, argv);

    // Setting the application version which is defined in johnny.pro
    app.setApplicationVersion(APP_VERSION);

    QSettings settings(
        QDir(QDir::home().filePath(".john")).filePath("johnny.conf"),
        QSettings::IniFormat);
    QString     settingLanguage = settings.value("Language").toString();
    Translator &translator      = Translator::getInstance();

    // If no language is saved : default behavior is to use system language
    if(settingLanguage.isEmpty())
    {
        QString systemLanguage = QLocale::languageToString(QLocale().language());
        translator.translateApplication(&app, systemLanguage);
        settings.setValue("Language", translator.getCurrentLanguage().toLower());
    }
    else
    {
        // Use the language specified in the settings
        translator.translateApplication(&app, settingLanguage);
    }

    MainWindow window(settings);
    window.show();

    return app.exec();
}
