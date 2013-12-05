#-------------------------------------------------
#
# Project created by QtCreator 2013-10-04T21:49:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sim8085
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    instructionset.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    instructions.txt
