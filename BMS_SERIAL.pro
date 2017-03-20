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
    mythread.cpp \
    canmessagemodel.cpp \
    bms/bms.c \
    bms/config.c \
    bms/Hachiko.c \
    bms/log.c \
    bms/tom.c \
    global.cpp \
    canmessagedlg.cpp \
    cansendscheduler.cpp

HEADERS  += mainbmswindow.h \
    ControlCAN.h \
    mythread.h \
    canmessagemodel.h \
    bms/bms.h \
    bms/charge.h \
    bms/config.h \
    bms/error.h \
    bms/Hachiko.h \
    bms/log.h \
    global.h \
    canmessagedlg.h \
    cansendscheduler.h \
    qcanmessage.h

FORMS    += mainbmswindow.ui \
    canmessagedlg.ui

LIBS += $$PWD/ControlCAN.lib

RESOURCES += \
    bms_serial.qrc
