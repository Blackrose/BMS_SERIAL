#-------------------------------------------------
#
# Project created by QtCreator 2017-03-07T10:10:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BMS_SERIAL
TEMPLATE = app


SOURCES += main.cpp\
        mainbmswindow.cpp

HEADERS  += mainbmswindow.h \
    ControlCAN.h

FORMS    += mainbmswindow.ui

LIBS += $$PWD/ControlCAN.lib
