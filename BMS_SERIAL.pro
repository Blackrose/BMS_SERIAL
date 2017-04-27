#-------------------------------------------------
#
# Project created by QtCreator 2017-03-07T10:10:05
#
#-------------------------------------------------

QT       += core gui
QT       += network

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
    cansendscheduler.cpp \
    my_debug/comserv/tdarray.c \
    my_debug/comserv/tdstr.c \
    my_debug/comserv/tdstring.c \
    my_debug/js/tdmalloc.c \
    my_debug/tdevice/file.c

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
    qcanmessage.h \
    my_debug/comserv/comserv.h \
    my_debug/comserv/tdarray.h \
    my_debug/comserv/tdstr.h \
    my_debug/comserv/tdstring.h \
    my_debug/js/tdmalloc.h \
    my_debug/platabs/tcomdef.h \
    my_debug/tdevice/file.h \
    my_debug/tdevice/tdevice.h

FORMS    += mainbmswindow.ui \
    canmessagedlg.ui

LIBS += $$PWD/ControlCAN.lib
LIBS += -lpthread libwsock32 libws2_32

RESOURCES += \
    bms_serial.qrc

DISTFILES += \
    my_debug/degSvr.exe \
    bms/ontom.cfg
