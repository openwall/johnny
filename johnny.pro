CONFIG   += qt

## Default build is debug
#CONFIG -= release
#CONFIG += debug

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = johnny

TEMPLATE = app

SOURCES +=  main.cpp\
            mainwindow.cpp \
            filetablemodel.cpp \
            translator.cpp \
            johnprocess.cpp \
            hashtypechecker.cpp \
            textlabel.cpp \
            johnhandler.cpp \
            johnattack.cpp \
            menu.cpp \
            tabwidget.cpp

HEADERS  += mainwindow.h \
            filetablemodel.h \
            translator.h \
            johnprocess.h \
            hashtypechecker.h \
            textlabel.h \
            johnhandler.h \
            johnattack.h \
            menu.h \
            tabwidget.h

FORMS    += mainwindow.ui

TRANSLATIONS    = translations/johnny_fr.ts
CODECFORTR      = UTF-8

RESOURCES += resources/resources.qrc

OTHER_FILES += README \
               LICENSE \
               INSTALL \
               CHANGELOG

# To build universal binary or .dmg on OS X, uncomment next line. It should work on all architecture.
#CONFIG += x86 ppc x86_64 ppc64
