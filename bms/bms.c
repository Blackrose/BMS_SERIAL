/*
 * BMS - CAN 通信过程
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <time.h>
#ifdef WIN32 // for windows
#include <winsock2.h>
//#pragma  comment(lib,"ws2_32.lib")
//#pragma comment(lib, "controlcan.lib")
#else // for linux
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#endif
#include <sys/types.h>


#include "charge.h"
#include "Hachiko.h"
#include "bms.h"
#include "config.h"
#include "log.h"
#include "error.h"

#undef BMS_CC_LANG //不使用C++语言
#include "global.h"

#define ANALYSIS_ON
//#undef ANALYSIS_ON

#define SET_DATA
#undef SET_DATA

#define  TIMEOUT_ON
//#undef TIMEOUT_ON
#define TIMEOUT 5000

#define INIT 0xFF



// 数据包生成器信息
struct can_pack_generator generator[] = {
    {
    .stage      =  CHARGE_STAGE_HANDSHACKING,
    .pgn        =  PGN_CHM,//0x002600,
    .prioriy    =  6,
    .datalen    =  3,
    .period     =  250,
    .heartbeat   =  0,
    .mnemonic   =  "CHM",
    .can_silence = 0,
    .can_tolerate_silence = TIMEOUT,
    .can_counter = 0
    },
    {
    .stage      =  CHARGE_STAGE_HANDSHACKING,
    .pgn        =  PGN_BHM,//0x002700,
    .prioriy    =  6,
    .datalen    =  2,
    .period     =  250,
    .heartbeat   =  0,
    .mnemonic   =  "BHM",
    .can_silence = 0,
    .can_tolerate_silence = TIMEOUT,
    .can_counter = 0
    },
    {
    .stage      =  CHARGE_STAGE_IDENTIFICATION,
    .pgn        =  PGN_CRM,//0x000100,
    .prioriy    =  6,
    .datalen    =  8,
    .period     =  250,
    .heartbeat   =  0,
    .mnemonic   =  "CRM",
    .can_silence = 0,
    .can_tolerate_silence = TIMEOUT,
    .can_counter = 0
    },
    {
    .stage      =  CHARGE_STAGE_IDENTIFICATION,
    .pgn        =  PGN_BRM,//0x000200,
    .prioriy    =  6,
    .datalen    =  41,
    .period     =  250,
    .heartbeat   =  0,
    .mnemonic   =  "BRM",
    .can_silence = 0,
    .can_tolerate_silence = TIMEOUT,
    .can_counter = 0
    },
    {
    .stage      =  CHARGE_STAGE_CONFIGURE,
    .pgn        =  PGN_BCP,//0x000600,
    .prioriy    =  6,
    .datalen    =  13,
    .period     =  500,
    .heartbeat   =  0,
    .mnemonic   =  "BCP",
    .can_silence = 0,
    .can_tolerate_silence = TIMEOUT,
    .can_counter = 0
    },
    {
    .stage      =  CHARGE_STAGE_CONFIGURE,
    .pgn        =  PGN_CTS,//0x000700,
    .prioriy    =  6,
    .datalen    =  7,
    .period     =  500,
    .heartbeat   =  0,
    .mnemonic   =  "CTS",
    .can_silence = 0,
    .can_tolerate_silence = TIMEOUT,
    .can_counter = 0
    },
    {
    .stage      =  CHARGE_STAGE_CONFIGURE,
    .pgn        =  PGN_CML,//0x000800,
    .prioriy    =  6,
    .datalen    =  8,//6
    .period     =  250,
    .heartbeat   =  0,
    .mnemonic   =  "CML",
    .can_silence = 0,
    .can_tolerate_silence = TIMEOUT,
    .can_counter = 0
    },
    {
    .stage      =  CHARGE_STAGE_CONFIGURE,
    .pgn        =  PGN_BRO,//0x00900,
    .prioriy    =  4,
    .datalen    =  1,
    .period     =  250,
    .heartbeat   =  0,
    .mnemonic   =  "BRO",
    .can_silence = 0,
    .can_tolerate_silence = TIMEOUT,
    .can_counter = 0
    },
    {
    .stage      =  CHARGE_STAGE_CONFIGURE,
    .pgn        =  PGN_CRO,//0x00A00,
    .prioriy    =  4,
    .datalen    =  1,
    .period     =  250,
    .heartbeat   =  0,
    .mnemonic   =  "CRO",
    .can_silence = 0,
    .can_tolerate_silence = TIMEOUT,
    .can_counter = 0
    },
    {
    .stage      =  CHARGE_STAGE_CHARGING,
    .pgn        =  PGN_BCL,//0x001000,
    .prioriy    =  6,
    .datalen    =  5,
    .period     =  50,
    .heartbeat   =  0,
    .mnemonic   =  "BCL",
    .can_silence = 0,
    .can_tolerate_silence = TIMEOUT,
    .can_counter = 0
    },
    {
    .stage      =  CHARGE_STAGE_CHARGING,
    .pgn        =  PGN_BCS,//0x001100,
    .prioriy    =  7,
    .datalen    =  9,
    .period     =  250,
    .heartbeat   =  0,
    .mnemonic   =  "BCS",
    .can_silence = 0,
    .can_tolerate_silence = TIMEOUT,
    .can_counter = 0
    },
    {
    .stage      =  CHARGE_STAGE_CHARGING,
    .pgn        =  PGN_CCS,//0x001200,
    .prioriy    =  6,
    .datalen    =  8,//6
    .period     =  50,
    .heartbeat   =  0,
    .mnemonic   =  "CCS",
    .can_silence = 0,
    .can_tolerate_silence = TIMEOUT,
    .can_counter = 0
    },
    {
    .stage      =  CHARGE_STAGE_CHARGING,
    .pgn        =  PGN_BSM,//0x001300,
    .prioriy    =  6,
    .datalen    =  7,
    .period     =  250,
    .heartbeat   =  0,
    .mnemonic   =  "BSM",
    .can_silence = 0,
    .can_tolerate_silence = TIMEOUT,
    .can_counter = 0
    },
    {
    .stage      =  CHARGE_STAGE_CHARGING,
    .pgn        =  PGN_BMV,//0x001500,
    .prioriy    =  7,
    .datalen    =  8,
    .period     =  10,
    .heartbeat   =  0,
    .mnemonic   =  "BMV",
    .can_silence = 0,
    .can_tolerate_silence = TIMEOUT,
    .can_counter = 0
    },
    {
    .stage      =  CHARGE_STAGE_CHARGING,
    .pgn        =  PGN_BMT,//0x001600,
    .prioriy    =  7,
    .datalen    =  8,
    .period     =  10,
    .heartbeat   =  0,
    .mnemonic   =  "BMT",
    .can_silence = 0,
    .can_tolerate_silence = TIMEOUT,
    .can_counter = 0
    },
    {
    .stage      =  CHARGE_STAGE_CHARGING,
    .pgn        =  PGN_BSP,//0x001700,
    .prioriy    =  7,
    .datalen    =  8,
    .period     =  10,
    .heartbeat   =  0,
    .mnemonic   =  "BSP",
    .can_silence = 0,
    .can_tolerate_silence = TIMEOUT,
    .can_counter = 0
    },
    {
    .stage      =  CHARGE_STAGE_CHARGING,
    .pgn        =  PGN_BST,//0x001900,
    .prioriy    =  4,
    .datalen    =  4,
    .period     =  10,
    .heartbeat   =  0,
    .mnemonic   =  "BST",
    .can_silence = 0,
    .can_tolerate_silence = TIMEOUT,
    .can_counter = 0
    },
    {
    .stage      =  CHARGE_STAGE_CHARGING,
    .pgn        =  PGN_CST,//0x001A00,
    .prioriy    =  4,
    .datalen    =  4,
    .period     =  10,
    .heartbeat   =  0,
    .mnemonic   =  "CST",
    .can_silence = 0,
    .can_tolerate_silence = TIMEOUT,
    .can_counter = 0
    },
    {
    .stage      =  CHARGE_STAGE_DONE,
    .pgn        =  PGN_BSD,//0x001C00,
    .prioriy    =  6,
    .datalen    =  7,
    .period     =  250,
    .heartbeat   =  0,
    .mnemonic   =  "BSD",
    .can_silence = 0,
    .can_tolerate_silence = TIMEOUT,
    .can_counter = 0
    },
    {
    .stage      =  CHARGE_STAGE_DONE,
    .pgn        =  PGN_CSD,//0x001D00,
    .prioriy    =  6,
    .datalen    =  8,//5,
    .period     =  250,
    .heartbeat   =  0,
    .mnemonic   =  "CSD",
    .can_silence = 0,
    .can_tolerate_silence = TIMEOUT,
    .can_counter = 0
    },
    {
    .stage      =  CHARGE_STAGE_ANY,
    .pgn        =  PGN_BEM,//0x001E00,
    .prioriy    =  2,
    .datalen    =  4,
    .period     =  250,
    .heartbeat   =  0,
    .mnemonic   =  "BEM",
    .can_silence = 0,
    .can_tolerate_silence = TIMEOUT,
    .can_counter = 0
    },
    {
    .stage      =  CHARGE_STAGE_ANY,
    .pgn        =  PGN_CEM,//0x001F00,
    .prioriy    =  2,
    .datalen    =  4,
    .period     =  250,
    .heartbeat   =  0,
    .mnemonic   =  "CEM",
    .can_silence = 0,
    .can_tolerate_silence = TIMEOUT,
    .can_counter = 0
    }
};

#if 0
// CAN 数据包统计结构
struct bms_statistics statistics[] = {
    {
    .can_pgn = PGN_CRM,
    .can_silence = 0,
    .can_tolerate_silence = 0,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BRM,
    .can_silence = 0,
    .can_tolerate_silence = 250,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BCP,
    .can_silence = 0,
    .can_tolerate_silence = 500,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_CTS,
    .can_silence = 0,
    .can_tolerate_silence = 0,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_CML,
    .can_silence = 0,
    .can_tolerate_silence = 0,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BRO,
    .can_silence = 0,
    .can_tolerate_silence = 250,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_CRO,
    .can_silence = 0,
    .can_tolerate_silence = 0,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BCL,
    .can_silence = 0,
    .can_tolerate_silence = 50,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BCS,
    .can_silence = 0,
    .can_tolerate_silence = 250,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_CCS,
    .can_silence = 0,
    .can_tolerate_silence = 0,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BSM,
    .can_silence = 0,
    .can_tolerate_silence = 250,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BMV,
    .can_silence = 0,
    .can_tolerate_silence = 1000,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BMT,
    .can_silence = 0,
    .can_tolerate_silence = 1000,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BSP,
    .can_silence = 0,
    .can_tolerate_silence = 1000,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BST,
    .can_silence = 0,
    .can_tolerate_silence = 10,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_CST,
    .can_silence = 0,
    .can_tolerate_silence = 0,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BSD,
    .can_silence = 0,
    .can_tolerate_silence = 250,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_CSD,
    .can_silence = 0,
    .can_tolerate_silence = 0,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BEM,
    .can_silence = 0,
    .can_tolerate_silence = 0,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_CEM,
    .can_silence = 0,
    .can_tolerate_silence = 0,
    .can_counter = 0
    }
};
#endif

// 数据包超时心跳包, 定时器自动复位, 一个单位时间一次
void Hachiko_packet_heart_beart_notify_proc(Hachiko_EVT evt, void *_private,
                            const struct Hachiko_food *self)
{
    if (evt == HACHIKO_TIMEOUT ) {
        int i = 0;
        struct can_pack_generator *thiz;
        struct can_pack_generator *me;
        for ( i = 0;
              (unsigned int)i < sizeof(generator) / sizeof(struct can_pack_generator); i++ ) {
            thiz = &generator[i];
            if ( thiz->stage == task->bms_stage ) {
                if ( thiz->heartbeat < thiz->period ) {
                    //thiz->heartbeat += 10;
                    thiz->heartbeat += 1;
                } else {
                    thiz->heartbeat = thiz->period;
                }
            } else {
                thiz->heartbeat = 0;
            }
        }

        /*
         * 为了能够侦探到接受数据包的超时事件，需要在这里进行一个计数操作
         * 当can_silence 计数大于等于 can_tolerate_silence 时认为对应数据包接收超时，需要在BMS逻辑主线程
         * 中做相应处理.
         *
         * BEM和CEM不在超时统计范围内
         */
#if 0
        for ( i = 0;
              (unsigned int)i < (sizeof(generator) / sizeof(struct can_pack_generator) ) - 2; i++ ) {
            me = &generator[i];
            me->can_silence ++;
            if ( me->can_tolerate_silence < me->can_silence ) {
                switch (task->bms_stage) {
                case CHARGE_STAGE_HANDSHACKING:
                    if (me->pgn != PGN_BRM) break;
                    if (bit_read(task, F_GUN_1_PHY_CONN_STATUS)) {
                        if ( !bit_read(task, S_BMS_COMM_DOWN) ) {
                            bit_set(task, S_BMS_COMM_DOWN);
                            log_printf(WRN, "BMS: 握手阶段BMS通信"RED("故障"));
                            task->bms_stage = CHARGE_STAGE_HANDSHACKING;
                        }
                    }
                    break;
                case CHARGE_STAGE_CONFIGURE:
                    if (me->pgn != PGN_BCP) break;
                    if (bit_read(task, F_GUN_1_PHY_CONN_STATUS)) {
                        if ( !bit_read(task, S_BMS_COMM_DOWN) ) {
                            bit_set(task, S_BMS_COMM_DOWN);
                            log_printf(WRN, "BMS: 配置阶段BMS通信"RED("故障"));
                            task->bms_stage = CHARGE_STAGE_HANDSHACKING;
                        }
                    }
                    break;
                case CHARGE_STAGE_CHARGING:
                    if (me->pgn != PGN_BCL) break;
                    if (bit_read(task, F_GUN_1_PHY_CONN_STATUS)) {
                        if ( !bit_read(task, S_BMS_COMM_DOWN) ) {
                            bit_set(task, S_BMS_COMM_DOWN);
                            log_printf(WRN, "BMS: 充电阶段BMS通信"RED("故障"));
                            task->bms_stage = CHARGE_STAGE_HANDSHACKING;
                        }
                    }
                    break;
                case CHARGE_STAGE_DONE:
                    if (me->pgn != PGN_BSD) break;
                    if (bit_read(task, F_GUN_1_PHY_CONN_STATUS)) {
                        if ( !bit_read(task, S_BMS_COMM_DOWN) ) {
                            bit_set(task, S_BMS_COMM_DOWN);
                            log_printf(WRN, "BMS: 充电完成阶段BMS通信"RED("故障"));
                            task->bms_stage = CHARGE_STAGE_HANDSHACKING;
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }
#endif
    }
}

void set_packet_TP_CM_RTS(int png_num,struct event_struct* param){
    /* request a connection. TP.CM_RTS
     * byte[1]: 0x10
     * byte[2:3]: 消息大小，字节数目
     * byte[4]: 全部数据包的数目
     * byte[5]: 0xFF
     * byte[6:8]: PGN
     */
    log_printf(INF, "BMS: "RED("TP_CM_RTS"));
    param->buff.tx_buff[0] = CAN_TP_CM_RTS_CONTROL;
    param->buff.tx_buff[1] = generator[png_num].datalen%256;//0x29;//
    param->buff.tx_buff[2] = generator[png_num].datalen/256;//0x00;//
    param->buff.tx_buff[3] = (generator[png_num].datalen -1)/7 + 1; // (41-1)/7+1=6 0x06;//
    param->buff.tx_buff[4] = 0xFF;
    param->buff.tx_buff[5] = generator[png_num].pgn&0x0000FF;
    param->buff.tx_buff[6] = ((generator[png_num].pgn&0x00FF00) >> 8);//注意优先级
    param->buff.tx_buff[7] = ((generator[png_num].pgn&0xFF0000) >> 16);
    //task->can_tp_param.tp_size = generator[png_num].datalen;

    param->buff_payload = 8;//后期修改
    param->can_id = CAN_TP_CM_ID;
    param->evt_param = EVT_RET_OK;
}
void set_packet_TP_DT(int png_num,struct event_struct* param){
    /* TP.CM_DT
     *
     * byte[1]: 序列号（1、2、3、4...）
     * byte[2:8]: 数据（7字节），不足为FF
     */
    log_printf(INF, "BMS: "RED("TP_DT"));
//    param->buff.tx_buff[0] = 0x01;
//    param->buff.tx_buff[1] = 0x00;
//    param->buff.tx_buff[2] = 0x01;
//    param->buff.tx_buff[3] = 0x01;
//    param->buff.tx_buff[4] = 0x01;
//    param->buff.tx_buff[5] = 0x80;
//    param->buff.tx_buff[6] = 0x0C;
//    param->buff.tx_buff[7] = 0x04;
    log_printf(INF, "BMS: tp_pack_num==%d",task->can_tp_param.tp_pack_num);
    log_printf(INF, "BMS: tp_size==%d",task->can_tp_param.tp_size);

    if(task->can_tp_param.tp_pack_nr == ((task->can_tp_param.tp_size -1)/7+1)){//一次传输完成
        if(task->can_tp_param.tp_pack_num <= ((task->can_tp_param.tp_size-1)/7+1)){

            param->buff.tx_buff[0] = task->can_tp_param.tp_pack_num;
            //memcpy(&param->buff.tx_buff[1], &task->can_buff_in[(task->can_tp_param.tp_pack_num -1)*7], 7);
            memcpy(&param->buff.tx_buff[1], &task->can_tp_param.tx_buff[(task->can_tp_param.tp_pack_num -1)*7], 7);
            param->buff_payload = 8;//后期修改
            param->can_id = CAN_TP_DT_ID;
            task->can_tp_param.tp_pack_num ++;
            param->evt_param = EVT_RET_OK;
        }else{
            log_printf(INF, "BMS: 111tp_pack_num==%d",task->can_tp_param.tp_pack_num);
        }
    }else if(task->can_tp_param.tp_pack_nr == 0){
        //等待 不能马上接收数据包
    }else if((task->can_tp_param.tp_pack_nr != 0)&&(task->can_tp_param.tp_pack_nr <= ((task->can_tp_param.tp_size -1)/7+1))){//传送tp_pack_nr个
        if(task->can_tp_param.tp_pack_num <= ((task->can_tp_param.tp_size-1)/7+1)){
            param->buff.tx_buff[0] = task->can_tp_param.tp_pack_num;
            //memcpy(&param->buff.tx_buff[1], &task->can_buff_in[(task->can_tp_param.tp_pack_num -1)*7], 7);
            memcpy(&param->buff.tx_buff[1], &task->can_tp_param.tx_buff[(task->can_tp_param.tp_pack_num -1)*7], 7);
            param->buff_payload = 8;//后期修改
            param->can_id = CAN_TP_DT_ID;
            task->can_tp_param.tp_pack_num ++;
            param->evt_param = EVT_RET_OK;
            task->can_tp_param.tp_pack_nr --;//传送一个自减一个
        }
    }


}

/*
 * 本可以将串口通信，SOCKET通信方式也纳入该事件函数，但本着CAN通信优先的原则，暂时将
 * CAN通信时间数据独立出来进行处理。
 *
 * CAN数据包消息处理函数，必须是可重入函数
 * 该函数处于所有需要互斥对象的临界点，有多个线程会直接调用该函数。
 */
static int can_packet_callback(
        struct charge_task * thiz, EVENT_CAN ev, struct event_struct* param)
{
    switch ( ev ) {
    case EVENT_CAN_INIT:
        // 事件循环函数初始化
        thiz->can_bms_status = CAN_NORMAL;
        thiz->can_heart_beat.Hachiko_notify_proc=
                Hachiko_packet_heart_beart_notify_proc;
        Hachiko_new(&thiz->can_heart_beat, HACHIKO_AUTO_FEED, 4, NULL);
        log_printf(INF, "BMS: CHARGER now stage to "RED("CHARGE_STAGE_INVALID"));
        thiz->bms_stage = CHARGE_STAGE_INVALID;
        break;
    case EVENT_CAN_RESET:
        // 事件循环函数复位
        // 当发生通信中断时会收到该事件
        break;
    case EVENT_INVALID:
        // 无效事件，事件填充
        break;
    case EVENT_RX_DONE:
        // 数据包接受成功了
        about_packet_reciev_done(thiz, param);
        break;
    case EVENT_RX_ERROR:
        // 数据包接受失败了
        break;
    case EVENT_TX_FAILS:
        // 数据包发送失败了
        log_printf(DBG_LV0, "BMS: packet sent error");
        break;
    case EVENT_TX_DONE:
        // 数据包发送完成了
        log_printf(DBG_LV0, "BMS: packet sent. %08X", param->can_id);
        //param->can_id = param->can_id >> 8;
        about_packet_transfer_done(thiz,param);
        break;
    case EVENT_TX_PRE:
        // 决定是否要发送刚刚准备发送的数据包
        param->evt_param = EVT_RET_OK;
        break;
    case EVENT_TX_REQUEST:
        /*
         * 在这里进行CAN数据包的发送处理
         * 进行数据包发送的条件是：充电枪物理连接正常，进入车辆识别过程，或充电过程。
         *
         * 数据包的发送，优先级最高的是错误报文输出，若是遇到周期性发送的数据包在发送
         * 时序上有重叠的问题，那么在这里的处理方式是，先到先处理，例如在若干个循环内
         * 数据包A，B的发送周期是10ms, 30ms，那么A，B的发送时时序应该是如下方式
         * T (ms)      数据包
         * |             |
         * 0             A
         * 0  + $$       B
         * |             |
         * |             |
         * 10            A
         * |             |
         * |             |
         * 20            A
         * |             |
         * |             |
         * 30            A
         * 30 + $$       B
         * |             |
         * |             |
         * ...         ...
         *
         * $$ 表示最小的循环控制周期，一般来说是绝对小于数据包的发送周期的
         * 所以会有如下的控制逻辑结构
         * if ( ... ) {
         * } else if ( ... ) {
         * } else if ( ... ) {
         * } else ;
         * 在若干个循环控制周期内，数据包都能按照既定的周期发送完成.
         */
        switch ( thiz->bms_stage ) {
        case CHARGE_STAGE_INVALID:
            param->evt_param = EVT_RET_ERR;
            break;
        case CHARGE_STAGE_HANDSHACKING:
            if ( generator[I_BHM].heartbeat >= generator[I_BHM].period ) {
                gen_packet_PGN9984(thiz, param);
                generator[I_BHM].heartbeat = 0;
            }
            if ( generator[I_BEM].heartbeat >= generator[I_BEM].period ) {
                gen_packet_PGN7680(thiz, param);
                generator[I_BEM].heartbeat = 0;
            }
            break;
        case CHARGE_STAGE_IDENTIFICATION:
            if ( generator[I_BRM].heartbeat >= generator[I_BRM].period ) {
                thiz->can_bms_status = (CAN_TP_WR | CAN_TP_RTS);
            }
            break;
        case CHARGE_STAGE_CONFIGURE:
            if ( generator[I_BCP].heartbeat >= generator[I_BCP].period ) {
                thiz->can_bms_status = (CAN_TP_WR | CAN_TP_RTS);
            }
            else if ( generator[I_BRO].heartbeat >= generator[I_BRO].period ) {
                gen_packet_PGN2304(thiz, param);
                generator[I_BRO].heartbeat = 0;
            }
            if ( generator[I_BEM].heartbeat >= generator[I_BEM].period ) {
                gen_packet_PGN7680(thiz, param);
                generator[I_BEM].heartbeat = 0;
            }
            break;
        case CHARGE_STAGE_CHARGING:
            if ( generator[I_BCL].heartbeat >= generator[I_BCL].period ) {
                gen_packet_PGN4096(thiz, param);
                generator[I_BCL].heartbeat = 0;
            }
            else if ( generator[I_BCS].heartbeat >= generator[I_BCS].period ) {
                thiz->can_bms_status = (CAN_TP_WR | CAN_TP_RTS);
            }else if ( generator[I_BSM].heartbeat >= generator[I_BSM].period ) {
                if(bit_read(thiz,F_CHARGER_CCS)){
                    gen_packet_PGN4864(thiz, param);
                }
                generator[I_BSM].heartbeat = 0;
            }/*else if ( generator[I_BMV].heartbeat >= generator[I_BMV].period ) {
                gen_packet_PGN5376(thiz, param);
                generator[I_BMV].heartbeat = 0;
            }else if ( generator[I_BMT].heartbeat >= generator[I_BMT].period ) {
                gen_packet_PGN5632(thiz, param);
                generator[I_BMT].heartbeat = 0;
            }*/else if ( generator[I_BST].heartbeat >= generator[I_BST].period ) {
                if(bit_read(thiz,F_BMS_STOP) || bit_read(thiz,F_CHARGER_CST)){//充电主动结束，或接受充电机充电中止报文
                    gen_packet_PGN6400(thiz, param);
                }
                generator[I_BST].heartbeat = 0;
            }
            if ( generator[I_BEM].heartbeat >= generator[I_BEM].period ) {
                gen_packet_PGN7680(thiz, param);
                generator[I_BEM].heartbeat = 0;
            }
            break;
        case CHARGE_STAGE_DONE:
            if ( generator[I_BSD].heartbeat >= generator[I_BSD].period ) {
                gen_packet_PGN7168(thiz, param);
                generator[I_BSD].heartbeat = 0;
            }
            if ( generator[I_BEM].heartbeat >= generator[I_BEM].period ) {
                gen_packet_PGN7680(thiz, param);
                generator[I_BEM].heartbeat = 0;
            }
            break;
        default:
            break;
        }
        break;
    case EVENT_TX_TP_RTS:
    {
        //串口处于连接管理状态时，将会收到该传输数据报请求。
        log_printf(INF, "BMS: EVENT_TX_TP_RTS.");

#if 1
        switch ( thiz->bms_stage ) {
            case CHARGE_STAGE_INVALID:
                param->evt_param = EVT_RET_ERR;
                break;
            case CHARGE_STAGE_IDENTIFICATION:
                if ( generator[I_BRM].heartbeat >= generator[I_BRM].period ) {
                    gen_packet_PGN512(thiz, param);
                    set_packet_TP_CM_RTS(I_BRM,param);
                    generator[I_BRM].heartbeat = 0;
                }
                if ( generator[I_BEM].heartbeat >= generator[I_BEM].period ) {
                    gen_packet_PGN7680(thiz, param);
                    generator[I_BEM].heartbeat = 0;
                }
                break;
            case CHARGE_STAGE_CONFIGURE:
                if ( generator[I_BCP].heartbeat >= generator[I_BCP].period ) {
                    gen_packet_PGN1536(thiz, param);
                    set_packet_TP_CM_RTS(I_BCP,param);
                    generator[I_BCP].heartbeat = 0;
                }
                if ( generator[I_BEM].heartbeat >= generator[I_BEM].period ) {
                    gen_packet_PGN7680(thiz, param);
                    generator[I_BEM].heartbeat = 0;
                }
                break;
        case CHARGE_STAGE_CHARGING:
            if ( generator[I_BCS].heartbeat >= generator[I_BCS].period ) {
                gen_packet_PGN4352(thiz, param);
                set_packet_TP_CM_RTS(I_BCS,param);
                generator[I_BCS].heartbeat = 0;
            }
            default:
                break;
        }
#endif
        //set_packet_TP_CM_RTS(I_BRM,param);
        /* request a connection. TP.CM_RTS
         * byte[1]: 0x10
         * byte[2:3]: 消息大小，字节数目
         * byte[4]: 全部数据包的数目
         * byte[5]: 0xFF
         * byte[6:8]: PGN
         */
//        param->buff.tx_buff[0] = CAN_TP_CM_RTS_CONTROL;
//        param->buff.tx_buff[1] = 0x29;
//        param->buff.tx_buff[2] = 0x00;
//        param->buff.tx_buff[3] = 0x06;
//        param->buff.tx_buff[4] = 0xFF;
//        param->buff.tx_buff[5] = 0x00;
//        param->buff.tx_buff[6] = 0x02;
//        param->buff.tx_buff[7] = 0x00;
//        param->buff_payload = 8;
//        param->can_id = CAN_TP_CM_ID;
//        param->evt_param = EVT_RET_OK;
    }
        break;
    case EVENT_TX_TP_CTS:
    {
        /* TP.CM_CTS
         *
         * byte[1]: 0x11
         * byte[2]: 可发送的数据包个数
         * byte[3]: 下一个要发送的数据包编号
         * byte[4:5]: 0xFF
         * byte[6:8]: PGN
         */
    }
        break;
    case EVENT_TX_TP_DT:
    {
        log_printf(INF, "BMS: EVENT_TX_TP_DT.");
#if 1
        switch ( thiz->bms_stage ) {
            case CHARGE_STAGE_INVALID:
                param->evt_param = EVT_RET_ERR;
                break;
            case CHARGE_STAGE_IDENTIFICATION:
                if ( generator[I_BRM].heartbeat >= generator[I_BRM].period/25/*10ms*/ ) {
                    set_packet_TP_DT(I_BRM,param);
                    generator[I_BRM].heartbeat = 0;
                }
                //set_packet_TP_DT(I_BRM,param);
                break;
            case CHARGE_STAGE_CONFIGURE:
                if ( generator[I_BCP].heartbeat >= generator[I_BCP].period/50/*10ms*/ ) {
                    set_packet_TP_DT(I_BCP,param);
                    generator[I_BCP].heartbeat = 0;
                }
                break;
            case CHARGE_STAGE_CHARGING:
                if ( generator[I_BCS].heartbeat >= generator[I_BCS].period/25/*10ms*/ ) {
                    gen_packet_PGN4352(thiz, param);
                    set_packet_TP_DT(I_BCS,param);
                    generator[I_BCS].heartbeat = 0;
                }
            default:
                break;
        }
#endif
        /* TP.CM_DT
         *
         * byte[1]: 序列号（1、2、3、4...）
         * byte[2:8]: 数据（7字节），不足为FF
         */
//        param->buff.tx_buff[0] = 0x01;
//        param->buff.tx_buff[1] = 0x00;
//        param->buff.tx_buff[2] = 0x01;
//        param->buff.tx_buff[3] = 0x01;
//        param->buff.tx_buff[4] = 0x01;
//        param->buff.tx_buff[5] = 0x80;
//        param->buff.tx_buff[6] = 0x0C;
//        param->buff.tx_buff[7] = 0x04;
//        param->buff_payload = 8;
//        param->can_id = CAN_TP_DT_ID;
//        param->evt_param = EVT_RET_OK;
    }
        break;
    case EVENT_TX_TP_ACK:
    {        
        /*TP.CM_ACK
         *
         * byte[1]: 0x13
         * byte[2:3]: 消息大小，字节数目
         * byte[4]: 全部数据包数目
         * byte[5]: 0xFF
         * byte[6:8]: PGN
         */
        param->evt_param = EVT_RET_OK;
    }
        break;
    case EVENT_TX_TP_ABRT:
        //TP.CM_ABORT
        break;
    default:
        break;
    }

    return 0;
}
// CAN数据包发送完成
int about_packet_transfer_done(struct charge_task *thiz,
                             struct event_struct *param)
{
    switch ((param->can_id & 0x00FF0000) >> 8) {
        case PGN_BHM:
            break;
        case PGN_BRO:// 0x000900, BMS 准备就绪报文
            log_printf(INF, "BMS: PGN_BRO ");
            break;
        case PGN_BCL:// 0x001000, BMS 电池充电需求报文
            log_printf(INF, "BMS: PGN_BCL ");
            //thiz->can_bms_status = (CAN_TP_WR | CAN_TP_RTS);//准备发送BCS
            bit_set(thiz, F_BMS_BCL);
            break;
        case PGN_BST:
            log_printf(INF, "BMS: PGN_BST ");
            bit_set(thiz,F_BMS_BST);//发送中止报文
            break;
    }
    return ERR_OK;
}
// CAN数据包接受完成
int about_packet_reciev_done(struct charge_task *thiz,
                             struct event_struct *param)
{
    switch ( param->can_id & 0x00FF00 ) {
    //==============================================握手阶段=================//
    case PGN_CHM:// 0x002600,握手报文
        generator[I_CHM].can_counter ++;
        generator[I_CHM].can_silence = 0;
        if ( bit_read(task, S_BMS_COMM_DOWN) ) {
            log_printf(INF, "BMS: BMS 通信"GRN("恢复"));
        }
        bit_clr(task, S_BMS_COMM_DOWN);

        if ( param->buff_payload == 8 ) {
            memcpy(&thiz->charger_handshake, param->buff.rx_buff, 8);
        } else if ( param->buff_payload == sizeof(struct pgn9728_CHM) ) {
            memcpy(&thiz->charger_handshake,
                   param->buff.rx_buff, sizeof(struct pgn9728_CHM));
        }
        if(thiz->charger_handshake.spn2600_charger_version[0] == 0x00 &&
                thiz->charger_handshake.spn2600_charger_version[1] == 0x01 &&
                    thiz->charger_handshake.spn2600_charger_version[2] == 0x01){
            log_printf(INF, "BMS: BMS change stage to "RED("CHARGE_STAGE_HANDSHACKING"));
            thiz->bms_stage = CHARGE_STAGE_HANDSHACKING;
        }else{
            log_printf(ERR, "BMS: BMS handshake err "RED("PGN_CHM"));
            thiz->bms_err_stage = CHARGE_STAGE_ERR_HANDSHACKING;
        }
        break;
    //==============================================辨识阶段=================//
    case PGN_CRM :// 0x000100,辨识报文
        generator[I_CRM].can_counter ++;
        generator[I_CRM].can_silence = 0;
        if ( bit_read(task, S_BMS_COMM_DOWN) ) {
            log_printf(INF, "BMS: BMS 通信"GRN("恢复"));
        }
        bit_clr(task, S_BMS_COMM_DOWN);

        if ( param->buff_payload == 8 ) {
            memcpy(&thiz->charger_info, param->buff.rx_buff, 8);
        } else if ( param->buff_payload == sizeof(struct pgn256_CRM) ) {
            memcpy(&thiz->charger_info,
                   param->buff.rx_buff, sizeof(struct pgn256_CRM));
        }
        if( thiz->charger_info.spn2560_recognize == BMS_NOT_RECOGNIZED ){
            log_printf(WRN,
                  "BMS not recognized .");
            bit_clr(thiz, F_BMS_RECOGNIZED);
            log_printf(INF, "BMS: BMS change stage to "RED("CHARGE_STAGE_IDENTIFICATION"));
            thiz->bms_stage = CHARGE_STAGE_IDENTIFICATION;
            thiz->can_bms_status = (CAN_TP_WR | CAN_TP_RTS);
            break;
        }else if( thiz->charger_info.spn2560_recognize == BMS_RECOGNIZED ){
            bit_set(thiz, F_VEHICLE_RECOGNIZED);
            log_printf(INF, "BMS: BMS change stage to "RED("CHARGE_STAGE_CONFIGURE"));
            thiz->bms_stage = CHARGE_STAGE_CONFIGURE;
            thiz->can_bms_status = (CAN_TP_WR | CAN_TP_RTS);
        }else{
            thiz->bms_err_stage = CHARGE_STAGE_ERR_IDENTIFICATION;
        }
        break;
    //==============================================参数阶段=================//
    case PGN_CTS :// 0x000700, 时间同步
        log_printf(INF, "BMS: PGN_CTS 0x000700");
        generator[I_CTS].can_counter ++;
        generator[I_CTS].can_silence = 0;
        if ( bit_read(task, S_BMS_COMM_DOWN) ) {
            log_printf(INF, "BMS: BMS 通信"GRN("恢复"));
        }
        bit_clr(task, S_BMS_COMM_DOWN);

        if ( param->buff_payload == 8 ) {
            memcpy(&thiz->charger_cts, param->buff.rx_buff, 8);
        } else if ( param->buff_payload == sizeof(struct pgn1792_CTS) ) {
            memcpy(&thiz->charger_cts,
                   param->buff.rx_buff, sizeof(struct pgn1792_CTS));
        }
        bit_set(thiz,F_CHARGER_CTS);
        break;
    case PGN_CML :// 0x000800, 充电机最大输出能力
        log_printf(INF, "BMS: PGN_CML 0x000800");
        generator[I_CML].can_counter ++;
        generator[I_CML].can_silence = 0;
        if ( bit_read(task, S_BMS_COMM_DOWN) ) {
            log_printf(INF, "BMS: BMS 通信"GRN("恢复"));
        }
        bit_clr(task, S_BMS_COMM_DOWN);

        if ( param->buff_payload == 8 ) {
            memcpy(&thiz->charger_cml, param->buff.rx_buff, 8);
        } else if ( param->buff_payload == sizeof(struct pgn2048_CML) ) {
            memcpy(&thiz->charger_cml,
                   param->buff.rx_buff, sizeof(struct pgn2048_CML));
        }
        bit_set(thiz,F_CHARGER_CML);
        break;
    case PGN_CRO :// 0x000A00, 充电机准备就绪
        log_printf(INF, "BMS: PGN_CRO 0x000A00");
        generator[I_CRO].can_counter ++;
        generator[I_CRO].can_silence = 0;
        if ( bit_read(task, S_BMS_COMM_DOWN) ) {
            log_printf(INF, "BMS: BMS 通信"GRN("恢复"));
        }
        bit_clr(task, S_BMS_COMM_DOWN);

        if ( param->buff_payload == 8 ) {
            memcpy(&thiz->charger_cro, param->buff.rx_buff, 8);
        } else if ( param->buff_payload == sizeof(struct pgn2560_CRO) ) {
            memcpy(&thiz->charger_cro,
                   param->buff.rx_buff, sizeof(struct pgn2560_CRO));
        }
        if(bit_read(thiz, F_BMS_READY )){
            if(thiz->charger_cro.spn2830_charger_ready_for_charge == CHARGER_NOT_READY_FOR_CHARGE){
                //bit_clr(thiz,F_BMS_READY);
                bit_clr(thiz,F_CHARGER_READY);
            }else if(thiz->charger_cro.spn2830_charger_ready_for_charge == CHARGER_READY_FOR_CHARGE){
                bit_set(thiz,F_CHARGER_READY);
                log_printf(INF, "BMS: BMS change stage to "RED("CHARGE_STAGE_CHARGING"));
                thiz->bms_stage = CHARGE_STAGE_CHARGING;
                thiz->can_bms_status = (CAN_TP_WR | CAN_TP_RTS);//准备发送BCS
            }else{
                thiz->bms_err_stage = CHARGE_STAGE_ERR_CONFIGURE;
            }
        }
        break;
    //==============================================充电阶段=================//
    case PGN_CCS :// 0x001200, 充电机充电状态
        log_printf(INF, "BMS: PGN_CCS 0x001200");
        generator[I_CCS].can_counter ++;
        generator[I_CCS].can_silence = 0;
        if ( bit_read(task, S_BMS_COMM_DOWN) ) {
            log_printf(INF, "BMS: BMS 通信"GRN("恢复"));
        }
        bit_clr(task, S_BMS_COMM_DOWN);

        if ( param->buff_payload == 8 ) {
            memcpy(&thiz->charger_ccs, param->buff.rx_buff, 8);
        } else if ( param->buff_payload == sizeof(struct pgn4608_CCS) ) {
            memcpy(&thiz->charger_ccs,
                   param->buff.rx_buff, sizeof(struct pgn4608_CCS));
        }
        bit_set(thiz,F_CHARGER_CCS);
        if(thiz->charger_ccs.spn3929_charger_status == CHARGER_ALLOW){
            bit_set(thiz,F_CHARGER_CCS);
        }else{

        }
        break;
    case PGN_CST :// 0x001A00, 充电机中止充电
        log_printf(INF, "BMS: PGN_CST 0x001A00");
        generator[I_CST].can_counter ++;
        generator[I_CST].can_silence = 0;
        if ( bit_read(task, S_BMS_COMM_DOWN) ) {
            log_printf(INF, "BMS: BMS 通信"GRN("恢复"));
        }
        bit_clr(task, S_BMS_COMM_DOWN);

        if ( param->buff_payload == 8 ) {
            memcpy(&thiz->charger_cst, param->buff.rx_buff, 8);
        } else if ( param->buff_payload == sizeof(struct pgn6656_CST) ) {
            memcpy(&thiz->charger_cst,
                   param->buff.rx_buff, sizeof(struct pgn6656_CST));
        }
        bit_set(thiz,F_CHARGER_CST);
        if(bit_read(thiz,F_BMS_BST)) {//收到充电机中止报文，且已经发送过中止报文
            log_printf(INF, "BMS: BMS change stage to "RED("CHARGE_STAGE_DONE"));
            thiz->bms_stage = CHARGE_STAGE_DONE;
        }
        break;
    //==============================================结束阶段=================//
    case PGN_CSD :// 0x001D00, 充电机统计数据
        log_printf(INF, "BMS: PGN_CSD 0x001D00");
        generator[I_CSD].can_counter ++;
        generator[I_CSD].can_silence = 0;
        if ( bit_read(task, S_BMS_COMM_DOWN) ) {
            log_printf(INF, "BMS: BMS 通信"GRN("恢复"));
        }
        bit_clr(task, S_BMS_COMM_DOWN);

        if ( param->buff_payload == 8 ) {
            memcpy(&thiz->charger_cst, param->buff.rx_buff, 8);
        } else if ( param->buff_payload == sizeof(struct pgn7424_CSD) ) {
            memcpy(&thiz->charger_cst,
                   param->buff.rx_buff, sizeof(struct pgn7424_CSD));
        }
        log_printf(INF, "BMS: BMS change stage to "RED("CHARGE_STAGE_ANY"));
        thiz->bms_stage = CHARGE_STAGE_ANY;
        break;
    case PGN_CEM :// 0x001F00, 充电机错误报文
        log_printf(INF, "BMS: PGN_CEM 0x001F00");
        generator[I_CEM].can_counter ++;
        generator[I_CEM].can_silence = 0;
        if ( bit_read(task, S_BMS_COMM_DOWN) ) {
            log_printf(INF, "BMS: BMS 通信"GRN("恢复"));
        }
        bit_clr(task, S_BMS_COMM_DOWN);

        if ( param->buff_payload == 8 ) {
            memcpy(&thiz->charger_cst, param->buff.rx_buff, 8);
        } else if ( param->buff_payload == sizeof(struct pgn7936_CEM) ) {
            memcpy(&thiz->charger_cst,
                   param->buff.rx_buff, sizeof(struct pgn7936_CEM));
        }

        break;


    case PGN_BHM :// 0x002700, BMS 握手报文
        break;
    case PGN_BRM :// 0x000200, BMS 车辆辨识报文
        log_printf(INF, "BMS: PGN_BRM 0x000200");
        break;
    case PGN_BCP :// 0x000600, BMS 配置报文
        log_printf(INF, "BMS: PGN_BCP 0x000600");
        break;
    case PGN_BRO :// 0x000900, BMS 充电准备就绪报文
        break;
    case PGN_BCL :// 0x001000, BMS 电池充电需求报文
        break;
    case PGN_BCS :// 0x001100, BMS 电池充电总状态报文
        log_printf(INF, "BMS: PGN_BCS 0x001100");
        if(thiz->can_bms_status == (CAN_TP_WR | CAN_TP_ACK)){
            bit_set(thiz,F_BMS_BCS);
        }
        break;
    case PGN_BSM :// 0x001300, 动力蓄电池状态信息报文
        break;
    case PGN_BMV :// 0x001500, 单体动力蓄电池电压报文
        log_printf(INF, "BMS: PGN_BMV fetched.");
        break;
    case PGN_BMT :// 0x001600, 单体动力蓄电池温度报文
        log_printf(INF, "BMS: PGN_BMT fetched.");
        break;
    case PGN_BSP :// 0x001700, 动力蓄电池预留报文
        log_printf(INF, "BMS: PGN_BSP fetched.");
        break;
    case PGN_BST :// 0x001900, BMS 中止充电报文
        log_printf(INF, "BMS: PGN_BST fetched.");
        break;
    case PGN_BSD :// 0x001C00, BMS 统计数据报文
        log_printf(INF, "BMS: PGN_BSD fetched.");
        break;
    case PGN_BEM :// 0x001E00, BMS 错误报文
        log_printf(INF, "BMS: PGN_BEM fetched.");
        break;
    default:
        log_printf(WRN, "BMS: un-recognized PGN %08X",
                   param->can_id);
        break;
    }
    return ERR_OK;
}

/*
 * CAN通信处于普通模式
 *
 * SAE J1939-21 Revised December 2006 版协议中规定
 * TP.CM.PGN 为 0x00EC00  连接管理
 * TP.DT.PGN 为 0x00EB00  数据传输
 * CAN 的数据包大小最大为8字节，因此需要传输大于等于9字节的数据包
 * 时需要使用连接管理功能来传输数据包。
 * 首先由数据发送方，发送一个数据发送请求包，请求包中包含的数据内容有
 * 消息的总长度，需要发送的数据包个数（必须大于1），最大的数据包编号，
 * 这个消息的PGN等
 */


// bms 通信 写 服务线程
// 提供bms通信服务
void *thread_bms_write_service(void *arg) ___THREAD_ENTRY___
{
    int *done = (int *)arg;
    int mydone = 0;
    int s = 0;
    int nbytes;
    VCI_CAN_OBJ frame;//[RX_BUFF_SIZE];
    VCI_ERR_INFO errinfo;
    struct event_struct param;
    unsigned char txbuff[TEMP_BUFF_SIZE];

    if ( done == NULL ) done = &mydone;

    log_printf(INF, "%s running...s=%d", __FUNCTION__, s);

    param.buff_payload = 0;
    param.evt_param = EVT_RET_INVALID;

    // 进行数据结构的初始化操作
    can_packet_callback(task, EVENT_CAN_INIT, &param);

    while ( ! *done ) {
        usleep(5000);

        /*
         * 写线程同时负责写数据和进行连接管理时的控制数据写出，这里需要对当前CAN的
         * 状态进行判定，当CAN处于CAN_NORMAL时进行普通的写操作，采用EVENT_TX_REQUEST,
         * 当CAN处于CAN_TP_RD时采用EVENT_TX_TP_CTS,EVENT_TX_TP_ACK
         * 当CAN处于CAN_TP_WR时采用EVENT_TX_TP_RTS,EVENT_TX_TP_DT
         */
        param.buff.tx_buff = txbuff;
        param.buff_size = sizeof(txbuff);
        param.evt_param = EVT_RET_INVALID;
        if ( task->can_bms_status & CAN_NORMAL ) {
            can_packet_callback(task, EVENT_TX_REQUEST, &param);
        } else if ( task->can_bms_status & CAN_TP_WR ) {//增加多数据包写
            log_printf(WRN, "BMS: CAN_TP_WRITE.");
            switch ( task->can_bms_status & 0xF0 ) {
            case CAN_TP_RTS:
                can_packet_callback(task, EVENT_TX_TP_RTS, &param);//请求发送数据包
                break;
            case CAN_TP_TX:// 数据发送中
                can_packet_callback(task, EVENT_TX_TP_DT, &param);//发送数据包中......
                break;
            case CAN_TP_ACK:
                can_packet_callback(task, EVENT_TX_TP_ACK, &param);//发送完成并接收应答
                break;
            default:
                log_printf(WRN, "BMS: can_bms_status crashed(%d).",
                           task->can_bms_status);
                continue;
                break;
            }
        } else if ( task->can_bms_status == CAN_INVALID ) {
            log_printf(DBG_LV0, "BMS: invalid can_bms_status: %d.",
                       task->can_bms_status);
            continue;
        } else {
            log_printf(DBG_LV0, "BMS: invalid can_bms_status: %d.",
                       task->can_bms_status);
            continue;
        }

        if ( EVT_RET_OK != param.evt_param ) {
            log_printf(DBG_LV0, "BMS: param.evt_param: %d.",
                       param.evt_param);
            continue;
        }

        param.evt_param = EVT_RET_INVALID;
        // 链接模式下的数据包发送不需要确认, 并且也不能被中止
        if ( task->can_bms_status == CAN_NORMAL ) {
            can_packet_callback(task, EVENT_TX_PRE, &param);
            if ( EVT_RET_TX_ABORT == param.evt_param ) {
                // packet sent abort.
                log_printf(DBG_LV0, "BMS: param.evt_param: EVT_RET_TX_ABORT.");
                continue;
            } else if ( EVT_RET_OK != param.evt_param ) {
                log_printf(DBG_LV0, "BMS: param.evt_param: not EVT_RET_OK.");
                continue;
            } else {
                // confirm to send.
                log_printf(DBG_LV0,"confirm to send.");
            }
        }
        frame.ExternFlag = 0x01;
        frame.RemoteFlag = 0x00;
        frame.SendType = 0x00;

        log_printf(DBG_LV0,"confirm to send111111111111.");

        /* 根据GB/T 27930-2011 中相关规定，BMS向充电机发送数据包有超过8字节的，
         * 在EVENT_TX_TP_RTS和EVENT_TX_TP_DT中进行连接管理通信。
         */
        if ( param.buff_payload <= 8 && param.buff_payload > 0 ) {
            frame.ID = param.can_id;
            frame.DataLen= param.buff_payload;
            memcpy(frame.Data, param.buff.tx_buff, 8);
            nbytes = VCI_Transmit(m_devtype,m_devind,m_cannum,&frame,TX_BUFF_SIZE);
            if(nbytes<=0){
                //注意：如果没有读到数据则必须调用此函数来读取出当前的错误码，
                //千万不能省略这一步（即使你可能不想知道错误码是什么）
                VCI_ReadErrInfo(m_devtype,m_devind,m_cannum,&errinfo);
                param.evt_param = EVT_RET_ERR;
                can_packet_callback(task, EVENT_TX_FAILS, &param);
                log_printf(ERR,"VCI_ReadErrInfo");
            }else{
                param.evt_param = EVT_RET_OK;
                can_packet_callback(task, EVENT_TX_DONE, &param);
                send_frame.ID = frame.ID;
                send_frame.DataLen = frame.DataLen;
                send_frame.ExternFlag = frame.ExternFlag;
                send_frame.RemoteFlag = frame.RemoteFlag;
                send_frame.SendType = frame.SendType;
                memcpy(send_frame.Data,frame.Data,frame.DataLen);
            }
        } else if ( param.buff_payload > 8 ) {
            // 大于8字节的数据包在这里处理，程序向后兼容
            log_printf(INF,"param.buff_payload > 8.");
        } else {
        }

        param.buff_payload = 0;
#if 0 //后期需要修改
        // 准备接收完成
        if ( task->can_bms_status == (CAN_TP_RD | CAN_TP_CTS) ) {
            task->can_bms_status = (CAN_TP_RD | CAN_TP_RX);
            log_printf(DBG_LV3, "BMS: ready for data transfer.");
        }
        // 应答结束
        if ( task->can_bms_status == (CAN_TP_RD | CAN_TP_ACK) ) {
            task->can_bms_status = CAN_NORMAL;
            log_printf(DBG_LV0, "BMS: connection closed normally.");
        }
        // 传输终止
        if ( task->can_bms_status == (CAN_TP_RD | CAN_TP_ABRT) ) {
            task->can_bms_status = CAN_NORMAL;
            log_printf(DBG_LV2, "BMS: connection aborted.");
        }
#endif
#if 0
        // 请求发送数据
        if ( task->can_bms_status == (CAN_TP_WR | CAN_TP_RTS) ) {
            task->can_bms_status = (CAN_TP_WR | CAN_TP_TX);
            log_printf(DBG_LV3, "BMS: ready for data transfer.");
        }
#else
        // 数据传送完成并且应答结束
        if ( task->can_bms_status == (CAN_TP_WR | CAN_TP_ACK) ) {
            task->can_bms_status = CAN_NORMAL;
            log_printf(DBG_LV0, "BMS: connection closed normally.");
        }
#endif
    }
    return NULL;
}

// CAN 数据发送报文
void Hachiko_CAN_TP_notify_proc(Hachiko_EVT evt, void *private,
                            const struct Hachiko_food *self)
{
    if ( evt == HACHIKO_TIMEOUT ) {
        log_printf(WRN, "BMS: CAN data transfer terminal due to time out.");
        task->can_bms_status = CAN_NORMAL;
    } else if ( evt == HACHIKO_DIE ) {

    }
}

// bms 通信 读 服务线程
// 提供bms通信服务
void *thread_bms_read_service(void *arg) ___THREAD_ENTRY___
{
    int *done = (int *)arg;
    int mydone = 0;
    int s = 0;

    VCI_CAN_OBJ frame;//[RX_BUFF_SIZE];//VCI_CAN_OBJ frameinfo[RX_BUFF_SIZE];
    VCI_ERR_INFO errinfo;
    int nbytes;

    struct event_struct param;
    // 用于链接管理的数据缓冲
    unsigned char tp_buff[2048];

    // 缓冲区数据字节数
    unsigned int tp_cnt = 0;
    // 数据包个数
    unsigned int tp_packets_nr = 0;
    // 数据包字节总数
    unsigned int tp_packets_size = 0;
    // 数据包编号
    unsigned int tp_packets_num = 0;
    // 数据包对应的PGN
    unsigned int tp_packet_PGN = 0;

    unsigned int dbg_packets = 0;

    task->can_tp_private.status = PRIVATE_INVALID;
    task->can_tp_bomb._private = (void *)&task->can_tp_private;

    if ( done == NULL ) done = &mydone;

    log_printf(INF, "BMS: %s running...s=%d", __FUNCTION__, s);

    param.buff.rx_buff = tp_buff;
    param.buff_size = sizeof(tp_buff);
    param.buff_payload = 0;
    while ( ! *done ) {
        usleep(5000);

        if ( task->can_bms_status  == CAN_INVALID ) {
            continue;
        }

        memset(&frame, 0, sizeof(frame));
        nbytes = VCI_Receive(m_devtype, m_devind, m_cannum, &frame, RX_BUFF_SIZE, RX_WAIT_TIME/*ms*/);
        if(nbytes<=0){
            //注意：如果没有读到数据则必须调用此函数来读取出当前的错误码，
            //千万不能省略这一步（即使你可能不想知道错误码是什么）
            VCI_ReadErrInfo(m_devtype,m_devind,m_cannum,&errinfo);
            param.evt_param = EVT_RET_ERR;
            log_printf(DBG_LV3, "BMS: read frame error %x", frame.ID);
            can_packet_callback(task, EVENT_RX_ERROR, &param);
            continue;
        }
        else{
//            debug_log(DBG_LV1,
//                       "BMS: get %dst packet %08X:%02X%02X%02X%02X%02X%02X%02X%02X",
//                       dbg_packets,
//                       frame.ID,
//                       frame.Data[0],
//                       frame.Data[1],
//                       frame.Data[2],
//                       frame.Data[3],
//                       frame.Data[4],
//                       frame.Data[5],
//                       frame.Data[6],
//                       frame.Data[7]);
            receive_frame.ID = frame.ID;
            receive_frame.DataLen = frame.DataLen;
            receive_frame.ExternFlag = frame.ExternFlag;
            receive_frame.RemoteFlag = frame.RemoteFlag;
            receive_frame.SendType = frame.SendType;
            memcpy(receive_frame.Data,frame.Data,frame.DataLen);
        }

#if 1
        if ( (frame.ID & 0xFFFF) != CAN_RCV_ID_MASK ) {
            #if 0
            log_printf(DBG_LV0, "BMS: id not accept %x", frame.ID);
            #endif
            continue;
        }

        dbg_packets ++;

        debug_log(DBG_LV1,
                   "BMS: get %dst packet %08X:%02X%02X%02X%02X%02X%02X%02X%02X",
                   dbg_packets,
                   frame.ID,
                   frame.Data[0],
                   frame.Data[1],
                   frame.Data[2],
                   frame.Data[3],
                   frame.Data[4],
                   frame.Data[5],
                   frame.Data[6],
                   frame.Data[7]);

        if ( ((frame.ID & 0x00FF0000) >> 16 ) == 0xEC ) {
            // Connection managment
            if ( CAN_TP_CM_CTS_CONTROL == frame.Data[0] ) {
                /* TP.CM_CTS
                 *
                 * byte[1]: 0x11
                 * byte[2]: 可发送的数据包个数
                 * byte[3]: 下一个要发送的数据包编号
                 * byte[4:5]: 0xFF
                 * byte[6:8]: PGN
                 */
                tp_packets_nr = frame.Data[1];//当（task->can_tp_param.tp_pack_nr）fram.Data[1] == (task->can_tp_param.tp_size -1)/7+1  一次传输完成
                tp_packets_num = frame.Data[2];
                tp_packet_PGN = frame.Data[5] +
                        (frame.Data[6] << 8) + (frame.Data[7] << 16);
                task->can_tp_param.tp_pack_nr = tp_packets_nr;
                task->can_tp_param.tp_pack_num = tp_packets_num;
                //task->can_tp_param.tp_size = tp_packets_size;
                task->can_tp_param.tp_pgn = tp_packet_PGN;
                task->can_tp_param.tp_rcv_bytes = 0;
                task->can_tp_param.tp_rcv_pack_nr = 0;

                param.can_id = tp_packet_PGN;
                param.buff_payload = frame.DataLen;
                param.evt_param = EVT_RET_INVALID;
                can_packet_callback(task, EVENT_RX_DONE, &param);

                task->can_bms_status = (CAN_TP_WR | CAN_TP_TX);
            } else if ( CAN_TP_CM_ACK_CONTROL == frame.Data[0] ) {
                /* TP.CM_ACK
                 *
                 * byte[1]: 0x13
                 * byte[2:3]: 消息大小，字节数目
                 * byte[4]: 全部数据包数目
                 * byte[5]: 0xFF
                 * byte[6:8]: PGN
                 */
                task->can_tp_param.tp_rcv_bytes = frame.Data[2] * 256 + frame.Data[1];
                //task->can_tp_param.tp_rcv_pack_nr = frame.Data[3];

                param.can_id = tp_packet_PGN;
                param.buff_payload = frame.DataLen;
                param.evt_param = EVT_RET_INVALID;
                can_packet_callback(task, EVENT_RX_DONE, &param);


                task->can_bms_status = (CAN_TP_WR | CAN_TP_ACK);
                //待增加内容......
            }else if ( 0xFF == frame.Data[0] ) {
                /* TP.CM_ABORT
                 *
                 * byte[1]: 0xFF
                 * byte[2:5]: 0xFF
                 * byte[6:8]: PGN
                 */
                int *d = (int *)&frame.Data[0];
                log_printf(DBG_LV2, "BMS: %08X", *d);
            } else {
                //omited.
                int *d = (int *)&frame.Data[0];
                log_printf(DBG_LV3, "BMS: %08X", *d);
            }
        } else {
            param.can_id = (frame.ID & 0x00FF0000) >> 8;
            param.buff_payload = frame.DataLen;
            param.evt_param = EVT_RET_INVALID;
            log_printf(DBG_LV0, "BMS: param.evt_param:EVT_RET_INVALID %d.",
                       param.evt_param);
            memcpy((void * __restrict__)param.buff.rx_buff, frame.Data, 8);
            can_packet_callback(task, EVENT_RX_DONE, &param);
            log_printf(DBG_LV0, "BMS: read a frame done. %08X", frame.ID);
        }
#endif
        if ( task->can_bms_status == CAN_NORMAL ) {
        } else if ( task->can_bms_status == CAN_TP_RD ) {
            // CAN通信处于连接管理模式
        }
    }

    return NULL;
}

/* 充电阶段改变事件处理过程
 *
 * 充电阶段发生改变后，会影响系统中数据包的发送逻辑等其他数据通信逻辑
 * 因此需要用单独的事件捕捉函数来进行处理，作相应的状态机修改
 **/
void on_charge_stage_change(CHARGE_STAGE_CHANGE_EVENT evt,
                            CHARGE_STAGE pre,
                            struct charge_task *thiz)
{
    switch (evt) {
    case STAGE_CHANGE_2_INVALID:
        break;
    case STAGE_CHANGE_2_HANDSHAKING:
        break;
    case STAGE_CHANGE_2_CHARGING:
        break;
    case STAGE_CHANGE_2_DONE:
        break;
    case STAGE_CHANGE_2_EXIT:
        break;
    case STAGE_CHANGE_2_ABORT:
    default:
        break;
    }
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------

// 握手-CHM-充电机握手报文


// 握手-BHM-BMS握手报文
int gen_packet_PGN9984(struct charge_task * thiz, struct event_struct* param)
{
    struct can_pack_generator *gen = &generator[I_CHM];

//    if ( 0 == bit_read(thiz, F_BMS_RECOGNIZED) ) {
//        param->buff.tx_buff[0] = BMS_NOT_RECOGNIZED;
//    } else {
//        param->buff.tx_buff[0] = BMS_RECOGNIZED;
//        bit_set(thiz, F_VEHICLE_RECOGNIZED);
//    }

    memset(param->buff.tx_buff, INIT, sizeof(struct pgn9984_BHM));
#ifdef SET_DATA
    set_data_bms_PGN9984(thiz);
#endif
    memcpy(param->buff.tx_buff, &thiz->bms_handshake, sizeof(struct pgn9984_BHM));

    param->buff_payload = gen->datalen;
    param->can_id = gen->prioriy << 26 | gen->pgn << 8 | CAN_TX_ID_MASK | CAN_EFF_FLAG;

    param->evt_param = EVT_RET_OK;

    generator[I_CHM].can_counter ++;

    return 0;
}
int set_data_bms_PGN9984(struct charge_task * thiz)
{
    memset(&thiz->bms_handshake, INIT, sizeof(struct pgn9984_BHM));
    thiz->bms_handshake.spn2601_bms_max_vol = 750;
    return 0;
}



// 握手-CRM-充电机辨识报文


// 握手-BRM-BMS辨识报文
int gen_packet_PGN512(struct charge_task * thiz, struct event_struct* param)
{
    log_printf(INF, "BMS: "RED("gen_packet_PGN512"));
    struct can_pack_generator *gen = &generator[I_BRM];

    memset(param->buff.tx_buff, INIT, sizeof(struct pgn512_BRM));
#ifdef SET_DATA
    set_data_bms_PGN512(thiz);
#endif
    memcpy(param->buff.tx_buff, &thiz->vehicle_info, sizeof(struct pgn512_BRM));

    param->buff_payload = gen->datalen;
    param->can_id = gen->prioriy << 26 | gen->pgn << 8 | CAN_TX_ID_MASK | CAN_EFF_FLAG;

    param->evt_param = EVT_RET_OK;

    generator[I_BRM].can_counter ++;//I_CRM

    task->can_tp_param.tp_size = gen->datalen;
    //memcpy(task->can_buff_in, &thiz->vehicle_info, sizeof(struct pgn512_BRM));//copy 到临时数据结构中
    memcpy(task->can_tp_param.tx_buff, &thiz->vehicle_info, sizeof(struct pgn512_BRM));//copy 到临时数据结构中

    memset(param->buff.tx_buff, INIT, sizeof(struct pgn512_BRM));
    log_printf(INF, "BMS: "RED("gen_packet_PGN512 end"));
    return 0;
}
int set_data_bms_PGN512(struct charge_task * thiz)
{
    memset(&thiz->vehicle_info, INIT, sizeof(struct pgn512_BRM));
    thiz->vehicle_info.spn2565_bms_version[0] = 0x00;
    thiz->vehicle_info.spn2565_bms_version[1] = 0x01;
    thiz->vehicle_info.spn2565_bms_version[2] = 0x00;
    thiz->vehicle_info.spn2566_battery_type = 0x03;
    thiz->vehicle_info.spn2567_capacity = 850;
    thiz->vehicle_info.spn2568_volatage = 450;
    return 0;
}

//配置-BCP-动力蓄电池充电参数
int gen_packet_PGN1536(struct charge_task * thiz, struct event_struct* param)
{
    log_printf(INF, "BMS: "RED("gen_packet_PGN1536"));
    struct can_pack_generator *gen = &generator[I_BCP];

    memset(param->buff.tx_buff, INIT, sizeof(struct pgn1536_BCP));
#ifdef SET_DATA
    set_data_bms_PGN1536(thiz);
#endif
    memcpy(param->buff.tx_buff, &thiz->bms_config_info, sizeof(struct pgn1536_BCP));

    param->buff_payload = gen->datalen;
    param->can_id = gen->prioriy << 26 | gen->pgn << 8 | CAN_TX_ID_MASK | CAN_EFF_FLAG;

    param->evt_param = EVT_RET_OK;

    generator[I_BRM].can_counter ++;//I_CRM

    task->can_tp_param.tp_size = gen->datalen;
    //memcpy(task->can_buff_in, &thiz->bms_config_info, sizeof(struct pgn1536_BCP));//copy 到临时数据结构中
    memcpy(task->can_tp_param.tx_buff, &thiz->bms_config_info, sizeof(struct pgn1536_BCP));//copy 到临时数据结构中

    memset(param->buff.tx_buff, INIT, sizeof(struct pgn1536_BCP));
    log_printf(INF, "BMS: "RED("gen_packet_PGN1536 end"));
    return 0;
}
int set_data_bms_PGN1536(struct charge_task * thiz)
{
    memset(&thiz->bms_config_info, INIT, sizeof(struct pgn1536_BCP));
    thiz->bms_config_info.spn2816_max_charge_volatage_single_battery = 425;
    thiz->bms_config_info.spn2817_max_charge_current = 1200;
    thiz->bms_config_info.spn2818_total_energy = 400;
    thiz->bms_config_info.spn2819_max_charge_voltage = 4000;
    thiz->bms_config_info.spn2820_max_temprature = 100;
    thiz->bms_config_info.spn2821_soc = 400;
    thiz->bms_config_info.spn2822_total_voltage = 3860;
    return 0;
}
// 配置-CTS-充电机发送时间同步信息
// 配置-CML-充电机最大输出能力

// 配置-BRO-BMS输出准备就绪状态
int gen_packet_PGN2304(struct charge_task * thiz, struct event_struct* param)
{
    log_printf(INF, "BMS: "RED("gen_packet_PGN2304"));
    struct can_pack_generator *gen = &generator[I_BRO];

    memset(param->buff.tx_buff, INIT,  gen->datalen);
#ifdef SET_DATA
    set_data_bms_PGN2304(thiz);
#endif
    if(thiz->bms_bro.spn2829_bms_ready_for_charge == BMS_NOT_READY_FOR_CHARGE){
        bit_clr(thiz, F_BMS_READY);
    }else if(thiz->bms_bro.spn2829_bms_ready_for_charge == BMS_READY_FOR_CHARGE){
        bit_set(thiz, F_BMS_READY);
    }else{
        log_printf(WRN, "BMS: wrong can package data.");
    }
    memcpy(param->buff.tx_buff, &thiz->bms_bro, sizeof(struct pgn2304_BRO));

    param->buff_payload = gen->datalen;
    param->can_id = gen->prioriy << 26 | gen->pgn << 8 | CAN_TX_ID_MASK | CAN_EFF_FLAG;
    param->evt_param = EVT_RET_OK;
    generator[I_BRO].can_counter ++;

    return 0;
}

int set_data_bms_PGN2304(struct charge_task * thiz)
{
    memset(&thiz->bms_bro, INIT, sizeof(struct pgn2304_BRO));
    if(bit_read(thiz, F_CHARGER_CTS ) && bit_read(thiz, F_CHARGER_CML)){
        thiz->bms_bro.spn2829_bms_ready_for_charge = BMS_READY_FOR_CHARGE;
    }else{
        thiz->bms_bro.spn2829_bms_ready_for_charge = BMS_NOT_READY_FOR_CHARGE;
    }
    return 0;
}

// 配置-CRO-充电机输出准备就绪状态

// 充电-BCL-电池充电需求
int gen_packet_PGN4096(struct charge_task * thiz, struct event_struct* param)
{
    log_printf(INF, "BMS: "RED("gen_packet_PGN4096"));
    struct can_pack_generator *gen = &generator[I_BCL];
    memset(param->buff.tx_buff, INIT,  gen->datalen);
#ifdef SET_DATA
    set_data_bms_PGN4096(thiz);
#endif
    memcpy(param->buff.tx_buff, &thiz->bms_bcl, sizeof(struct pgn4096_BCL));

    param->buff_payload = gen->datalen;
    param->can_id = gen->prioriy << 26 | gen->pgn << 8 | CAN_TX_ID_MASK | CAN_EFF_FLAG;
    param->evt_param = EVT_RET_OK;
    generator[I_BCL].can_counter ++;

    return 0;
}
int set_data_bms_PGN4096(struct charge_task * thiz)
{
    memset(&thiz->bms_bcl, INIT, sizeof(struct pgn4096_BCL));
    thiz->bms_bcl.spn3072_need_voltage = 5000;
    thiz->bms_bcl.spn3073_need_current = 1200;
    thiz->bms_bcl.spn3074_charge_mode = CHARGE_WITH_CONST_CURRENT;

    return 0;
}
// 充电-BCS-电池充电总状态
int gen_packet_PGN4352(struct charge_task * thiz, struct event_struct* param)
{
    log_printf(INF, "BMS: "RED("gen_packet_PGN4352"));
    struct can_pack_generator *gen = &generator[I_BCS];

    memset(param->buff.tx_buff, INIT, sizeof(struct pgn4352_BCS));
#ifdef SET_DATA
    set_data_bms_PGN4352(thiz);
#endif
    memcpy(param->buff.tx_buff, &thiz->bms_bcs, sizeof(struct pgn4352_BCS));

    param->buff_payload = gen->datalen;
    param->can_id = gen->prioriy << 26 | gen->pgn << 8 | CAN_TX_ID_MASK | CAN_EFF_FLAG;

    param->evt_param = EVT_RET_OK;

    generator[I_BCS].can_counter ++;

    task->can_tp_param.tp_size = gen->datalen;
    //memcpy(task->can_buff_in, &thiz->bms_bcs, sizeof(struct pgn4352_BCS));//copy 到临时数据结构中
    memcpy(task->can_tp_param.tx_buff, &thiz->bms_bcs, sizeof(struct pgn4352_BCS));//copy 到临时数据结构中

    memset(param->buff.tx_buff, INIT, sizeof(struct pgn4352_BCS));
    log_printf(INF, "BMS: "RED("gen_packet_PGN4352 end"));
    return 0;
}
int set_data_bms_PGN4352(struct charge_task * thiz)
{
    memset(&thiz->bms_bcs, INIT, sizeof(struct pgn4352_BCS));
    thiz->bms_bcs.spn3075_charge_voltage = 5000;
    thiz->bms_bcs.spn3076_charge_current = 1200;
    thiz->bms_bcs.spn3077_max_v_g_number = 2000;
    thiz->bms_bcs.spn3078_soc = 50;
    thiz->bms_bcs.spn3079_need_time = 20;
    return 0;
}
// 充电-CCS-充电机充电状态
// 充电-BSM-动力蓄电池状态信息
int gen_packet_PGN4864(struct charge_task * thiz, struct event_struct* param)
{
    log_printf(INF, "BMS: "RED("gen_packet_PGN4864"));
    struct can_pack_generator *gen = &generator[I_BSM];
    memset(param->buff.tx_buff, INIT,  gen->datalen);
#ifdef SET_DATA
    set_data_bms_PGN4864(thiz);
#endif
    memcpy(param->buff.tx_buff, &thiz->bms_bsm, sizeof(struct pgn4864_BSM));

    param->buff_payload = gen->datalen;
    param->can_id = gen->prioriy << 26 | gen->pgn << 8 | CAN_TX_ID_MASK | CAN_EFF_FLAG;
    param->evt_param = EVT_RET_OK;
    generator[I_BSM].can_counter ++;

    return 0;
}
int set_data_bms_PGN4864(struct charge_task * thiz)
{
    memset(&thiz->bms_bsm, INIT, sizeof(struct pgn4864_BSM));
    thiz->bms_bsm.sn_of_max_voltage_battery = 7;
    thiz->bms_bsm.max_temperature_of_battery = 40;
    thiz->bms_bsm.sn_of_max_temperature_point = 2;
    thiz->bms_bsm.min_temperature_of_battery = 2;
    thiz->bms_bsm.sn_of_min_temperature_point = 3;
    thiz->bms_bsm.remote_single = 0;

    return 0;
}

// 充电-BMV-单体动力蓄电池电压
int gen_packet_PGN5376(struct charge_task * thiz, struct event_struct* param)
{
    return 0;
}
// 充电-BMT-动力蓄电池温度
int gen_packet_PGN5632(struct charge_task * thiz, struct event_struct* param)
{
    return 0;
}
// 充电-BST-BMS中止充电
int gen_packet_PGN6400(struct charge_task * thiz, struct event_struct* param)
{
    log_printf(INF, "BMS: "RED("gen_packet_PGN6400"));
    struct can_pack_generator *gen = &generator[I_BST];
    memset(param->buff.tx_buff, INIT,  gen->datalen);
#ifdef SET_DATA
    set_data_bms_PGN6400(thiz);
#endif
    memcpy(param->buff.tx_buff, &thiz->bms_bst, sizeof(struct pgn6400_BST));

    param->buff_payload = gen->datalen;
    param->can_id = gen->prioriy << 26 | gen->pgn << 8 | CAN_TX_ID_MASK | CAN_EFF_FLAG;
    param->evt_param = EVT_RET_OK;
    generator[I_BST].can_counter ++;

    return 0;
}
int set_data_bms_PGN6400(struct charge_task * thiz)
{
    memset(&thiz->bms_bst, INIT, sizeof(struct pgn6400_BST));
    thiz->bms_bst.reason = 0;
    thiz->bms_bst.error = 0;
    thiz->bms_bst.fault = 0;
    return 0;
}

// 充电-CST-充电机中止充电
// 结束-BSD-BMS统计数据
int gen_packet_PGN7168(struct charge_task * thiz, struct event_struct* param)
{
    log_printf(INF, "BMS: "RED("gen_packet_PGN7168"));
    struct can_pack_generator *gen = &generator[I_BSD];
    memset(param->buff.tx_buff, INIT,  gen->datalen);
#ifdef SET_DATA
    set_data_bms_PGN7168(thiz);
#endif
    memcpy(param->buff.tx_buff, &thiz->bms_bsd, sizeof(struct pgn7168_BSD));

    param->buff_payload = gen->datalen;
    param->can_id = gen->prioriy << 26 | gen->pgn << 8 | CAN_TX_ID_MASK | CAN_EFF_FLAG;
    param->evt_param = EVT_RET_OK;
    generator[I_BSD].can_counter ++;

    return 0;
}
int set_data_bms_PGN7168(struct charge_task * thiz)
{
    memset(&thiz->bms_bsd, INIT, sizeof(struct pgn7168_BSD));
    thiz->bms_bsd.spn3601_stop_soc_status = 0;
    thiz->bms_bsd.spn3602_singal_battery_min_vol = 0;
    thiz->bms_bsd.spn3603_singal_battery_max_vol = 0;
    thiz->bms_bsd.spn3604_battery_min_temp = 0;
    thiz->bms_bsd.spn3605_battery_max_temp = 0;
    return 0;
}
// 结束-CSD-充电机统计数据

// 错误-CEM-充电机错误报文
// 错误-BEM-充电机错误报文
int gen_packet_PGN7680(struct charge_task * thiz, struct event_struct* param)
{
    log_printf(INF, "BMS: "RED("gen_packet_PGN7680"));
    struct can_pack_generator *gen = &generator[I_BEM];
    memset(param->buff.tx_buff, INIT,  gen->datalen);
#ifdef SET_DATA
    set_data_bms_PGN7680(thiz);
#endif
    memcpy(param->buff.tx_buff, &thiz->bms_bem, sizeof(struct pgn7680_BEM));

    param->buff_payload = gen->datalen;
    param->can_id = gen->prioriy << 26 | gen->pgn << 8 | CAN_TX_ID_MASK | CAN_EFF_FLAG;
    param->evt_param = EVT_RET_OK;
    generator[I_BEM].can_counter ++;
    return 0;
}
int set_data_bms_PGN7680(struct charge_task * thiz)
{
    memset(&thiz->bms_bem, INIT, sizeof(struct pgn7680_BEM));
    thiz->bms_bem.bem_crm = 0;
    thiz->bms_bem.bem_ccp = 0;
    thiz->bms_bem.bem_csd = 0;
    thiz->bms_bem.bem_cst = 0;
    return 0;
}
