#include "mainbmswindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainBMSWindow w;
    w.show();

    return a.exec();
}
