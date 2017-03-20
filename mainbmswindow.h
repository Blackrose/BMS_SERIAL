#ifndef MAINBMSWINDOW_H
#define MAINBMSWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QTimer>
#include <QEvent>
#include <QDateTime>
#include <QToolTip>
#include <QItemSelectionModel>
//#include <QGraphicsItem>
//#include <QGraphicsScene>
//#include <QGraphicsRectItem>
//#include <QGraphicsView>
//#include <QHelpEvent>


#ifdef WIN32 // for windows
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <time.h>
#define BMS_CC_LANG //使用C++语言
#include "global.h"
//#include "controlcan.h"
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
#include "mythread.h"
#include "canmessagemodel.h"
#include "qcanmessage.h"
#include "cansendscheduler.h"
#include "canmessagedlg.h"

namespace Ui {
class MainBMSWindow;
}

class MainBMSWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainBMSWindow(QWidget *parent = 0);
    ~MainBMSWindow();
    //bool event(QEvent *event);
    void my_tooltip();

    int m_cannum = 0;
    int m_devtype = 4;
    int m_devind = 0;

    //void ShowInfo(CString str,int code);

    int strtodata(unsigned char *str, unsigned char *data,int len,int flag);
    int chartoint(unsigned char chr, unsigned char *cint);

    QTimer can_timer;
    mythread mythread_can ;

private slots:
    void on_pushButton_connect_clicked();

    void on_comboBox_bps_currentIndexChanged(int index);

    void on_pushButton_discon_clicked();

    void on_checkBox_clicked(bool checked);


    void on_actionConnect_triggered();
    void on_actionDisconnect_triggered();
    void on_actionSend_triggered();
    void on_actionDelete_triggered();
    void on_actionEdit_triggered();
    void on_actionClearAll_triggered();
    void on_actionClear_triggered();
    void on_tableViewSend_doubleClicked(const QModelIndex& index);
    void on_actionNew_triggered();
    void on_actionAbout_triggered();
    void onCanbusError(quint32 error);

public slots:
    void slot_cantimer();
    void sendMessage(QCanMessage& msg);

private:
    Ui::MainBMSWindow *ui;

    CanMessageModel		*mReceiveModel;
    CanMessageModel 	*mSendModel;
    CanSendScheduler	*mScheduler;
};

#endif // MAINBMSWINDOW_H
