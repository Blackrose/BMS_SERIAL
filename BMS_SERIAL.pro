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
        mainbmswindow.cpp \
    bms/bms.c \
    bms/config.c \
    bms/Hachiko.c \
    bms/log.c \
    bms/tom.c \
    mythread.cpp \
    canmessagemodel.cpp

HEADERS  += mainbmswindow.h \
    ControlCAN.h \
    bms/bms.h \
    bms/charge.h \
    bms/config.h \
    bms/error.h \
    bms/Hachiko.h \
    bms/log.h \
    mythread.h \
    canmessagemodel.h

FORMS    += mainbmswindow.ui

LIBS += $$PWD/ControlCAN.lib
