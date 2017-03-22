#include "mainbmswindow.h"
#include "ui_mainbmswindow.h"

MainBMSWindow::MainBMSWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainBMSWindow)
{
    ui->setupUi(this);
    ui->pushButton_discon->setVisible(false);
    ui->groupBox_Charging_2->setVisible(false);
    ui->groupBox_Charging_3->setVisible(false);


    connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

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

    QItemSelectionModel *selectionModel = new QItemSelectionModel(mSendModel);
    ui->tableViewSend->setSelectionModel(selectionModel);


    my_tooltip();

    mScheduler = new CanSendScheduler(this);
    connect(mScheduler, SIGNAL(jobScheduled(QCanMessage&)), this, SLOT(sendMessage(QCanMessage&)));


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
    //辨识阶段
    ui->groupBox_SPN2560->setToolTip("辨识结果");
    ui->groupBox_SPN2561->setToolTip("充电机编号");

    ui->groupBox_SPN2565->setToolTip("BMS通讯协议版本号");
    ui->groupBox_SPN2566->setToolTip("电池类型");
    ui->groupBox_SPN2567->setToolTip("蓄电池系统额定容量/蓄电池系统额定总电压");

    //参数配置阶段
    ui->lineEdit_spn2816->setToolTip("单体动力蓄电池最高允许充电电压");
    ui->lineEdit_spn2817->setToolTip("最高允许充电电流");
    ui->lineEdit_spn2818->setToolTip("动力蓄电池标称总能量");
    ui->lineEdit_spn2819->setToolTip("最高允许充电总电压");
    ui->lineEdit_spn2820->setToolTip("最高允许温度");
    ui->lineEdit_spn2821->setToolTip("整车动力蓄电池荷电状态");
    ui->lineEdit_spn2822->setToolTip("整车动力蓄电池当前电池电压");

    ui->groupBox_CML_V->setToolTip("最高/低输出电压");
    ui->groupBox_CML_A->setToolTip("最大/小输出电流");

    //充电阶段
    ui->groupBox_SPN3072->setToolTip("需求电压");
    ui->groupBox_SPN3073->setToolTip("需求电流");
    ui->groupBox_SPN3074->setToolTip("充电模式");
    ui->groupBox_SPN3081->setToolTip("电压/电流输出");
    ui->groupBox_SPN3083->setToolTip("累计充电时间");
    ui->groupBox_SPN3929->setToolTip("充电允许/暂停");

    ui->groupBox_SPN3075->setToolTip("充电电压/电流测量值");
    ui->groupBox_SPN3077->setToolTip("最高单体动力蓄电池电压及组号");
    ui->groupBox_SPN3078->setToolTip("当前荷电状态");
    ui->groupBox_SPN3079->setToolTip("剩余充电时间");

    ui->lineEdit_spn3085->setToolTip("最高单体动力蓄电池电压所在编号");
    ui->lineEdit_spn3086->setToolTip("最高动力蓄电池温度");
    ui->lineEdit_spn3087->setToolTip("最高温度检测点编号");
    ui->lineEdit_spn3088->setToolTip("最低动力蓄电池温度");
    ui->lineEdit_spn3089->setToolTip("最低动力蓄电池温度检测点编号");

    ui->comboBox_spn3090->setToolTip("单体动力蓄电池电压过高/过低");
    ui->comboBox_spn3091->setToolTip("整车动力蓄电池荷电状态SOC过高/过低");
    ui->comboBox_spn3092->setToolTip("动力蓄电池充电过电流");
    ui->comboBox_spn3093->setToolTip("动力蓄电池温度过高");
    ui->comboBox_spn3094->setToolTip("动力蓄电池绝缘状态");
    ui->comboBox_spn3095->setToolTip("动力蓄电池输出连接器连接状态");
    ui->comboBox_spn3096->setToolTip("充电允许/禁止");

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

void MainBMSWindow::on_actionConnect_triggered()
{
#if 0
    ConnectDlg	dlg;

    if (!mConnection.device.isEmpty()) {
        dlg.setInterface(mConnection.device);
    }
    if (mConnection.appFilter) {
        dlg.setFilter(mConnection.filter);
    }
    if (mConnection.errorMask) {
        dlg.setErrorFrameMask(mConnection.errorMask);
    }
    if (dlg.exec() == QDialog::Accepted) {
        QString iface = dlg.interface();
        if (!iface.isEmpty()) {
            mSocket.close();
            if (mSocket.bind(iface, QIODevice::ReadWrite)) {
                ui.actionDisconnect->setEnabled(true);
                ui.labelBusError->setText("Ok");
                mConnection.device = iface;
                ui.labelDevice->setText(iface);
                if (dlg.filterMessages()) {
                    QPair<quint32, quint32> filter = dlg.filter();
                    mSocket.setMessageFilter(filter);
                    mConnection.filter = filter;
                    mConnection.appFilter = true;
                } else {
                    mConnection.filter = QPair<quint32, quint32>(0, 0);
                    mConnection.appFilter = false;
                }
                if (dlg.generateErrorFrames()) {
                    mSocket.setErrorFilter(dlg.errorFrameMask());
                    mConnection.errorMask = dlg.errorFrameMask();
                } else {
                    mConnection.errorMask = 0;
                }

            }
        }

    }
#endif
}

void MainBMSWindow::on_actionDisconnect_triggered()
{
    //mSocket.close();
    //ui.labelBusError->setText("offline");
    ui->actionDisconnect->setEnabled(false);
}

void MainBMSWindow::sendMessage(QCanMessage& msg)
{
//    mSocket.write(msg.frame.frame(), msg.frame.size());
    msg.count++;
    mSendModel->addMessage(msg);
}

void MainBMSWindow::on_actionSend_triggered()
{
    QModelIndex idx = ui->tableViewSend->selectionModel()->currentIndex();
    if (!idx.isValid()) return;

    QByteArray ba = idx.data(Qt::UserRole).toByteArray();
    QCanMessage *msg = reinterpret_cast<QCanMessage*>(ba.data());
    sendMessage(*msg);
}

void MainBMSWindow::on_actionNew_triggered()
{
    CanMessageDlg dlg;
    if (dlg.exec() == QDialog::Accepted) {
        QCanMessage msg = dlg.getMessage();
        mSendModel->addMessage(msg, false);
        mScheduler->addJob(msg);
    }
}

void MainBMSWindow::on_actionDelete_triggered()
{
    QModelIndex idx = ui->tableViewSend->selectionModel()->currentIndex();
    if (!idx.isValid()) return;

    QByteArray ba = idx.data(Qt::UserRole).toByteArray();
    QCanMessage *msg = reinterpret_cast<QCanMessage*>(ba.data());
    mSendModel->deleteMessage(*msg);
    mScheduler->removeJob(*msg);
}

void MainBMSWindow::on_actionClearAll_triggered()
{
    mSendModel->deleteAll();
    mScheduler->clearAll();
}

void MainBMSWindow::on_actionEdit_triggered()
{
    QModelIndex idx = ui->tableViewSend->selectionModel()->currentIndex();

    on_tableViewSend_doubleClicked(idx);
}

void MainBMSWindow::on_actionClear_triggered()
{
    mReceiveModel->deleteAll();
}

void MainBMSWindow::onCanbusError(quint32 error)
{
//    QString s;
//    if (error) {
//        if (error & CAN_ERR_TX_TIMEOUT)
//            s += "TX timeout ";
//        if (error & CAN_ERR_LOSTARB )
//            s += "| lost arb. ";
//        if (error & CAN_ERR_CRTL)
//            s += "| ctrl err. ";
//        if (error & CAN_ERR_PROT)
//            s += "| proto. viol. ";
//        if (error & CAN_ERR_TRX)
//            s += "| transceiver status ";
//        if (error & CAN_ERR_ACK)
//            s += "| no ACK ";
//        if (error & CAN_ERR_BUSOFF)
//            s += "| bus off ";
//        if (error & CAN_ERR_BUSERROR)
//            s += "| bus error ";
//        if (error & CAN_ERR_RESTARTED)
//            s += "| contrl. restarted";
//        if (s.startsWith('|'))
//            ui.labelBusError->setText(s.mid(2));
//        else
//            ui.labelBusError->setText(s);
//    } else {
//        ui.statusbar->showMessage(mSocket.errorString(), 3000);
//    }
}

void MainBMSWindow::on_tableViewSend_doubleClicked(const QModelIndex& index)
{

    if (index.isValid()) {
        QByteArray ba = index.data(Qt::UserRole).toByteArray();
        QCanMessage *msg = reinterpret_cast<QCanMessage*>(ba.data());
        if (index.column() == 0) {
            sendMessage(*msg);
        } else {
            CanMessageDlg dlg(*msg);
            if (dlg.exec() == QDialog::Accepted) {
                QCanMessage msg1 = dlg.getMessage();
                mSendModel->addMessage(msg1, false);
                mScheduler->addJob(msg1);
            }
        }
    }
}

void MainBMSWindow::on_actionAbout_triggered()
{
    QString versionInfo =   "<B>BMS_SERIAL</B><p>"
                            "Author: andy   <a href=mailto:andy.zhao@serialsystem.com>andy.zhao@serialsystem.com</a><p>";
#ifdef GITHASH
    versionInfo += "<p>Commit: <b>" XSTR(GITHASH) "</b>";
#endif
    QMessageBox::about ( this,
                    "About BMS_SERIAL", versionInfo);


}

void MainBMSWindow::slot_cantimer()
{
    //can_timer.stop();
    //mythread_can.start(); //bms_canbus();

    QCanMessage msg;

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
