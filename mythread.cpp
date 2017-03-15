#include "mythread.h"
#define BMS_CC_LANG
#include "bms/bms.h"
mythread::mythread(QObject *parent) : QThread(parent)
{
    //stop = false;
}

void mythread::run()
{
    bms_canbus();
}
void mythread::stop()
{
    //bms_canstop();
}
