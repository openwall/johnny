#-------------------------------------------------
#
# Project created by QtCreator 2011-03-31T22:42:36
#
#-------------------------------------------------

QT       += core gui

TARGET = johnny
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    tablemodel.cpp \
    johnParser.cpp \
    filetablemodel.cpp

HEADERS  += mainwindow.h \
    tablemodel.h \
    johnParser.h \
    filetablemodel.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources/resources.qrc

OTHER_FILES += \
    README \
    LICENSE
