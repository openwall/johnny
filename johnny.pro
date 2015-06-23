CONFIG   += qt

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
            menu.cpp

HEADERS  += mainwindow.h \
            filetablemodel.h \
            translator.h \
            johnprocess.h \
            hashtypechecker.h \
            textlabel.h \
            johnhandler.h \
            johnattack.h \
            menu.h

FORMS    += mainwindow.ui

TRANSLATIONS    = translations/johnny_fr.ts
CODECFORTR      = UTF-8

RESOURCES += resources/resources.qrc

OTHER_FILES += README \
               LICENSE


## Default build is debug
#CONFIG -= release
#CONFIG += debug
