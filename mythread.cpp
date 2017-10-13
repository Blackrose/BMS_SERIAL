#include "mythread.h"
#define BMS_CC_LANG
#include "bms/bms.h"
#include "mainbmswindow.h"
mythread::mythread(QObject *parent) : QThread(parent)
{
    stopped = false;
}

void mythread::run()
{
    /*while(!stopped)*/{
        bms_canbus();//函数中包含循环，so去得在此的循环
    }
    stopped = false;
}
void mythread::stop()
{
    stopped = true;
    bms_canstop();
    bms_exit();
    MainBMSWindow::ui_exit();
}
