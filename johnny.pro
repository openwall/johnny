CONFIG   += qt

## Default build is debug
#CONFIG -= release
#CONFIG += debug

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = johnny

TEMPLATE = app

SOURCES +=  src/main.cpp\
            src/mainwindow.cpp \
            src/filetablemodel.cpp \
            src/translator.cpp \
            src/johnprocess.cpp \
            src/hashtypechecker.cpp \
            src/textlabel.cpp \
            src/johnhandler.cpp \
            src/johnattack.cpp \
            src/menu.cpp \
            src/tabwidget.cpp

HEADERS  += src/mainwindow.h \
            src/filetablemodel.h \
            src/translator.h \
            src/johnprocess.h \
            src/hashtypechecker.h \
            src/textlabel.h \
            src/johnhandler.h \
            src/johnattack.h \
            src/menu.h \
            src/tabwidget.h

INCLUDEPATH += src/

FORMS    += forms/mainwindow.ui

TRANSLATIONS    = translations/johnny_fr.ts
CODECFORTR      = UTF-8

RESOURCES += resources/resources.qrc

OTHER_FILES += README \
               LICENSE \
               INSTALL \
               CHANGELOG

osx {
TARGET  = Johnny
ICON    = resources/icons/johnny.icns

# Build universal binary on OS X for Release target
release: CONFIG += x86 ppc x86_64 ppc64
}
