/*
 * Copyright (c) 2011 Shinnok <raydenxy at gmail.com>.
 * Copyright © 2011,2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>.  See LICENSE.
 */

#include <QCoreApplication>
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // We set application info up.
    // It is needed to be able to store settings easily.
    // TODO: Could we use openwall.com/john as domain?
    // TODO: Is that the right place for this?
    QCoreApplication::setOrganizationName("Openwall");
    QCoreApplication::setOrganizationDomain("openwall.com");
    QCoreApplication::setApplicationName("Johnny, the GUI for John the Ripper");

    QApplication app(argc, argv);

    //Translator needed to translate Qt's own strings
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
            QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    //Translator needed to translate Johnny's strings
    QTranslator johnnyTranslator;
    johnnyTranslator.load("johnny_" + QLocale::system().name(), ":/translations");
    app.installTranslator(&johnnyTranslator);

    MainWindow window;
    window.show();

    return app.exec();
}
