/*
 * Copyright (c) 2011 Shinnok <raydenxy at gmail.com>.
 * Copyright © 2011,2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>.  See LICENSE.
 */

#include "mainwindow.h"
#include "translator.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    // We set application info up.
    // It is needed to be able to store settings easily.
    // TODO: Could we use openwall.com/john as domain?
    // TODO: Is that the right place for this?
    QCoreApplication::setOrganizationName("Openwall");
    QCoreApplication::setOrganizationDomain("openwall.com");
    QCoreApplication::setApplicationName("Johnny");

    QApplication app(argc, argv);

    QSettings settings(QDir(QDir::home().filePath(".john")).filePath("johnny.conf"),
                QSettings::IniFormat);

    QString settingLanguage = settings.value("Language").toString();
    Translator& translator = Translator::getInstance();

    // If no language is saved : default behavior is to use system language
    if(settingLanguage.isEmpty())
    {
        QString systemLanguage =  QLocale::languageToString(QLocale().language());
        translator.translateApplication(&app, systemLanguage);
    }
    else
    {
        //Use the language specified in the settings
        translator.translateApplication(&app, settingLanguage);
    }

    MainWindow window(settings);
    window.show();

    return app.exec();
}
