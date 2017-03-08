#ifndef MAINBMSWINDOW_H
#define MAINBMSWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

#ifdef WIN32 // for windows
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <time.h>
#include "controlcan.h"
//#pragma comment(lib, "controlcan.lib")
#define msleep(ms)  Sleep(ms)
//typedef HANDLE pthread_t;
#else // for linux
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "controlcan.h"
#define msleep(ms)  usleep((ms)*1000)
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

namespace Ui {
class MainBMSWindow;
}

class MainBMSWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainBMSWindow(QWidget *parent = 0);
    ~MainBMSWindow();

    int m_cannum;
    int m_devtype;
    int m_devind;

    //void ShowInfo(CString str,int code);

    int strtodata(unsigned char *str, unsigned char *data,int len,int flag);
    int chartoint(unsigned char chr, unsigned char *cint);

private slots:
    void on_pushButton_connect_clicked();

    void on_comboBox_bps_currentIndexChanged(int index);

    void on_pushButton_discon_clicked();

private:
    Ui::MainBMSWindow *ui;
};

#endif // MAINBMSWINDOW_H
