CONFIG   += qt

## Default build is debug
#CONFIG -= release
#CONFIG += debug

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = johnny

# The application version
VERSION = 2.2

# Define the preprocessor macro to get the application version in our application.
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

TEMPLATE = app

SOURCES +=  src/main.cpp\
            src/mainwindow.cpp \
            src/translator.cpp \
            src/johnprocess.cpp \
            src/hashtypechecker.cpp \
            src/textlabel.cpp \
            src/johnhandler.cpp \
            src/johnattack.cpp \
            src/menu.cpp \
            src/tabwidget.cpp \
            src/johnsession.cpp \
            src/hashsortfilterproxymodel.cpp \
            src/passwordfilemodel.cpp \
            src/openotherformatfiledialog.cpp

HEADERS  += src/mainwindow.h \
            src/translator.h \
            src/johnprocess.h \
            src/hashtypechecker.h \
            src/textlabel.h \
            src/johnhandler.h \
            src/johnattack.h \
            src/menu.h \
            src/tabwidget.h \
            src/johnsession.h \
            src/hashsortfilterproxymodel.h \
            src/passwordfilemodel.h \
            src/openotherformatfiledialog.h

INCLUDEPATH += src/

FORMS    += forms/mainwindow.ui \
            forms/aboutwidget.ui \
            forms/openotherformatfiledialog.ui

TRANSLATIONS    = translations/johnny_fr.ts
CODECFORTR      = UTF-8

RESOURCES += resources/resources.qrc

OTHER_FILES += README \
               LICENSE \
               INSTALL \
               CHANGELOG \
               DEVELOPMENT

osx {
TARGET  = Johnny
ICON    = resources/icons/johnny.icns

# Build universal binary on OS X for Release target
release: CONFIG += x86 ppc x86_64 ppc64
}
