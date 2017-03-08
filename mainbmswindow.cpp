#include "mainbmswindow.h"
#include "ui_mainbmswindow.h"

MainBMSWindow::MainBMSWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainBMSWindow)
{
    ui->setupUi(this);
    ui->pushButton_discon->setVisible(false);
}

MainBMSWindow::~MainBMSWindow()
{
    delete ui;
}

void MainBMSWindow::on_pushButton_connect_clicked()
{
    VCI_INIT_CONFIG init_config;
    int index = 0,cannum = 0;
    m_devtype = 4;
    m_devind=index;
    m_cannum=cannum;

    init_config.AccCode=0x00000000;
    init_config.AccMask=0xffffffff;
    init_config.Filter=0x00;
    init_config.Mode=0x00;
    init_config.Timing0=0x00;
    init_config.Timing1=0x14;

    if(VCI_OpenDevice(m_devtype,index,0)!=STATUS_OK)
    {
        QMessageBox::warning(this,"打开设备失败!","警告",QMessageBox::Ok);
        return;
    }
    if(VCI_InitCAN(m_devtype,index,cannum,&init_config)!=STATUS_OK)
    {
        QMessageBox::warning(this,"初始化CAN失败!","警告",QMessageBox::Ok);
        VCI_CloseDevice(m_devtype,index);
        return;
    }

    if(VCI_StartCAN(m_devtype,m_devind,m_cannum)==STATUS_OK)
    {
        QMessageBox::information(this,"connect","启动成功",QMessageBox::Ok);
        ui->pushButton_connect->setVisible(false);
        ui->pushButton_discon->setVisible(true);
    }
    else
    {
        QMessageBox::information(this,"connect","启动失败",QMessageBox::Ok);
    }
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
