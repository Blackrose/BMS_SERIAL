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

#define INIT 0xFF
#define SET_DATA
//#undef SET_DATA

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

    //void ShowInfo(CString str,int code);

    int strtodata(unsigned char *str, unsigned char *data,int len,int flag);
    int chartoint(unsigned char chr, unsigned char *cint);

    QTimer can_timer;
    mythread mythread_can ;

    int oldvalue;
    QTimer bst_timer;

    void set_data_bms_PGN9984(struct charge_task * thiz);
    int get_spn2566_battery_type(int index);
    void set_data_bms_PGN512(struct charge_task * thiz);
    void set_data_bms_PGN1536(struct charge_task * thiz);
    int set_combobox_value(int index);
    void set_data_bms_PGN2304(struct charge_task * thiz);
    int set_charge_mode(int index);
    void set_data_bms_PGN4096(struct charge_task * thiz);
    void set_data_bms_PGN4352(struct charge_task * thiz);
    int analysis_data_bms_PGN4864(struct charge_task * thiz);
    int get_data_bms_PGN4864(struct charge_task * thiz);
    int set_combobox_data(int index);
    void set_data_bms_PGN4864(struct charge_task * thiz);
    void set_data_bms_PGN5376(struct charge_task * thiz);
    void set_data_bms_PGN5632(struct charge_task * thiz);
    void set_data_bms_PGN5888(struct charge_task * thiz);
    int get_data_bms_PGN6400_reason(struct charge_task * thiz);
    int get_data_bms_PGN6400_fault(struct charge_task * thiz);
    int get_data_bms_PGN6400_error(struct charge_task * thiz);
    void set_data_bms_PGN6400(struct charge_task * thiz);
    void set_data_bms_PGN7168(struct charge_task * thiz);
    void set_data_pgn();

    void show_data_charger_PGN9728(struct charge_task * thiz);//CHM
    void show_data_charger_PGN256(struct charge_task * thiz);//CRM
    void show_data_charger_PGN1792(struct charge_task * thiz);//CTS
    void show_data_charger_PGN2048(struct charge_task * thiz);//CML
    void show_data_charger_PGN2560(struct charge_task * thiz);//CRO
    void show_data_charger_PGN4608(struct charge_task * thiz);//CCS
    void show_data_charger_PGN6656(struct charge_task * thiz);//CST
    void show_data_charger_PGN7424(struct charge_task * thiz);//CSD
    void show_data_charger_PGN7680(struct charge_task * thiz);//BEM
    void show_data_charger_PGN7936(struct charge_task * thiz);//CEM
    void show_data_pgn();

    void SetValue(int );

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


    void on_pushButton_BHM_clicked();

    void on_pushButton_BRM_clicked();

    void on_pushButton_BCP_clicked();

    void on_pushButton_BCL_BCS_BSM_clicked();

    void on_pushButton_BSD_clicked();

    void slot_statustimer();

signals:
   void ValueChanged(int );

public slots:
    void slot_cantimer();
    void sendMessage(QCanMessage& msg);
    void ChangeValue(int );

private:
    Ui::MainBMSWindow *ui;

    CanMessageModel		*mReceiveModel;
    CanMessageModel 	*mSendModel;
    CanSendScheduler	*mScheduler;
};

#endif // MAINBMSWINDOW_H
