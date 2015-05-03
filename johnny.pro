#-------------------------------------------------
#
# Project created by QtCreator 2011-03-31T22:42:36
#
#-------------------------------------------------

CONFIG   += qt

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = johnny

TEMPLATE = app

SOURCES +=	main.cpp\
			mainwindow.cpp \
			tablemodel.cpp \
			filetablemodel.cpp \
                        translator.cpp \
                        johnprocess.cpp

HEADERS  += mainwindow.h \
			tablemodel.h \
			filetablemodel.h \
                        translator.h \
                        johnprocess.h

FORMS    += mainwindow.ui
TRANSLATIONS    = translations/johnny_fr.ts
CODECFORTR = UTF-8
RESOURCES += \
	resources/resources.qrc

OTHER_FILES += \
	README \
	LICENSE
## Default build is debug
#CONFIG -= release
#CONFIG += debug
