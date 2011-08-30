/*
 * Copyright (c) 2011 Shinnok <raydenxy at gmail.com>.  See LICENSE.
 */

#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // We set application info up.
    // It is needed to be able to store settings easily.
    // TODO: Is this info right?
    // TODO: Is that the right place for this?
    QCoreApplication::setOrganizationName("OpenWall");
    QCoreApplication::setOrganizationDomain("OpenWall.com");
    QCoreApplication::setApplicationName("Johnny, the GUI for John the Ripper");

    QApplication app(argc, argv);
    MainWindow window;
    window.show();

    return app.exec();
}
