#include "mainbmswindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainBMSWindow w;
    w.show();

    QFile file(":/ThemeRoller/style.qss");
    file.open(QFile::ReadOnly);
    qApp->setStyleSheet(file.readAll());
    file.close();
    return a.exec();
}
