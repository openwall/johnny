#-------------------------------------------------
#
# Project created by QtCreator 2011-03-31T22:42:36
#
#-------------------------------------------------

CONFIG   += qt

QT       += core gui widgets

TARGET = johnny

TEMPLATE = app

SOURCES +=	main.cpp\
			mainwindow.cpp \
			tablemodel.cpp \
			filetablemodel.cpp

HEADERS  += mainwindow.h \
			tablemodel.h \
			filetablemodel.h

FORMS    += mainwindow.ui

RESOURCES += \
	resources/resources.qrc

OTHER_FILES += \
	README \
	LICENSE

## Default build is debug
#CONFIG -= release
#CONFIG += debug
