#include "mainbmswindow.h"
#include "ui_mainbmswindow.h"


//int MainBMSWindow::m_cannum=0;
//int MainBMSWindow::m_devtype=4;
//int MainBMSWindow::m_devind=0;

MainBMSWindow::MainBMSWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainBMSWindow)
{
    ui->setupUi(this);
    ui->pushButton_discon->setVisible(false);

    connect(&can_timer,SIGNAL(timeout()),this,SLOT(slot_cantimer()));//版本校验下发参数
    can_timer.start(100);


    mReceiveModel = new CanMessageModel(CanMessageModel::Receive, this);
    mReceiveModel->setShowTrigger(false);
    ui->tableViewReceive->setModel(mReceiveModel);
    ui->tableViewReceive->horizontalHeader()->resizeSection(2, 200);
    ui->tableViewReceive->horizontalHeader()->resizeSection(0, 100);
    ui->tableViewReceive->horizontalHeader()->resizeSection(1, 60);
    ui->tableViewReceive->horizontalHeader()->resizeSection(3, 60);
    ui->tableViewReceive->horizontalHeader()->resizeSection(4, 60);
    ui->tableViewReceive->horizontalHeader()->resizeSection(5, 60);

    mSendModel = new CanMessageModel(CanMessageModel::Transmit, this);
    ui->tableViewSend->setModel(mSendModel);
    ui->tableViewSend->horizontalHeader()->resizeSection(2, 200);
    ui->tableViewSend->horizontalHeader()->resizeSection(0, 100);
    ui->tableViewSend->horizontalHeader()->resizeSection(1, 60);
    ui->tableViewSend->horizontalHeader()->resizeSection(3, 60);
    ui->tableViewSend->horizontalHeader()->resizeSection(4, 60);
    ui->tableViewSend->horizontalHeader()->resizeSection(5, 60);
    my_tooltip();
}

MainBMSWindow::~MainBMSWindow()
{
    delete ui;
}

#if 0 //可设置为全部显示注释
bool MainBMSWindow::event(QEvent *event)
 {
    //QGraphicsScene scene;
    //QTransform transform;
    //transform.rotate(+0.0);

     if (event->type() == QEvent::ToolTip) {
         QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
         //int index = itemAt(helpEvent->pos());
         //if (index != -1) {
         ui->groupBox_28->setToolTip("333333");
             QToolTip::showText(helpEvent->globalPos(),ui->groupBox_28->toolTip());
//         } else {
//             QToolTip::hideText();
//             event->ignore();
//         }

         return true;
     }
     return QWidget::event(event);
 }
#endif

void MainBMSWindow::my_tooltip()
{
    ui->groupBox_CML_V->setToolTip("最高/低输出电压");
    ui->groupBox_CML_A->setToolTip("最大/小输出电流");
    QToolTip::showText(ui->groupBox_CML_V->pos(),ui->groupBox_CML_V->toolTip());
    QToolTip::showText(ui->groupBox_CML_A->pos(),ui->groupBox_CML_A->toolTip());
}

void MainBMSWindow::on_pushButton_connect_clicked()
{
    VCI_INIT_CONFIG init_config;

    init_config.AccCode=0x00000000;
    init_config.AccMask=0xffffffff;
    init_config.Filter=0x00;
    init_config.Mode=0x00;
    init_config.Timing0=0x01;
    init_config.Timing1=0x1c;
#if 1
    if(VCI_OpenDevice(m_devtype,m_devind,0)!=STATUS_OK)
    {
        QMessageBox::warning(this,"打开设备失败!","警告",QMessageBox::Ok);
        return;
    }
    if(VCI_InitCAN(m_devtype,m_devind,m_cannum,&init_config)!=STATUS_OK)
    {
        QMessageBox::warning(this,"初始化CAN失败!","警告",QMessageBox::Ok);
        VCI_CloseDevice(m_devtype,m_devind);
        return;
    }

    if(VCI_StartCAN(m_devtype,m_devind,m_cannum)==STATUS_OK)
    {
        QMessageBox::information(this,"connect","启动成功",QMessageBox::Ok);
        ui->pushButton_connect->setVisible(false);
        ui->pushButton_discon->setVisible(true);
        mythread_can.start(); //bms_canbus();
    }
    else
    {
        QMessageBox::information(this,"connect","启动失败",QMessageBox::Ok);
    }
#endif
}

void MainBMSWindow::on_comboBox_bps_currentIndexChanged(int index)
{

}

void MainBMSWindow::on_pushButton_discon_clicked()
{
    VCI_CloseDevice(m_devtype,m_devind);
    ui->pushButton_connect->setVisible(true);
    ui->pushButton_discon->setVisible(false);
}

void MainBMSWindow::on_checkBox_clicked(bool checked)
{
    if(checked == TRUE){
        QMessageBox::information(this,"ready","准备握手",QMessageBox::Ok);
    }else{

    }
}


void MainBMSWindow::slot_cantimer()
{
    //can_timer.stop();
    //mythread_can.start(); //bms_canbus();

    CanMessageModel::QCanMessage msg;

    QDateTime now = QDateTime::currentDateTimeUtc();
    msg.time = now.toMSecsSinceEpoch();

    msg.frame = receive_frame;
    mReceiveModel->addMessage(msg);

    QString trace = QString("%1.%2      %3    %4    ").arg(msg.time / 1000, 16, 10, QLatin1Char(' ')).arg(msg.time % 1000, 3, 10, QLatin1Char('0')).arg(msg.frame.ID, 8, 16).arg(msg.frame.DataLen);
    for (int i = 0; i < msg.frame.DataLen; i++) {
        trace += QString("%1  ").arg(msg.frame.Data[i], 2, 16, QLatin1Char('0'));
    }

    msg.frame = send_frame;
    mSendModel->addMessage(msg);
}
