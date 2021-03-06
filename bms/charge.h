/*
 * 充电控制描述结构
 * All rights reserved Hangzhou Power Equipment Co., Ltd.
 * author: LiJie
 * email:  bedreamer@163.com
 * 2014-08-22
*/

#ifndef _CHARGE_INCLUDED_H_
#define _CHARGE_INCLUDED_H_

struct charge_task;

#include "Hachiko.h"
#include "bms.h"
#include "config.h"

// 无效时戳，初始化时默认赋值， 用于time_t默认值
#define INVALID_TIMESTAMP  0x00000000
// BMS 通信时的缓冲区
#define CAN_BUFF_SIZE         1024
/* 支持的BMS个数
 * 这里将支持的BMS配置成可变参数的形式。
 */
#define CONFIG_SUPPORT_BMS_NR 2
#define BUFF_NR               CONFIG_SUPPORT_BMS_NR
typedef enum {
    // 0# 充电枪
    GUN_SN0 = 0,
    // 1# 充电枪
    GUN_SN1 = 1,
    // 默认充电枪
    GUN_DEFAULT = 0,
    // 无效枪
    GUN_INVALID = 344,
    // 未指定充电枪
    GUN_UNDEFINE = 177
}CHARGE_GUN_SN;

#pragma pack(1)

typedef enum {
    // 分离的，未连接
    GUN_CONN_DETACHED = 0,
    // 连接保护
    GUN_CONN_PROTECTED = 1,
    // 连接异常
    GUN_CONN_EXCEPTION = 2,
    // 已连接
    GUN_CONN_CONNECTIVE=3
}GUN_CONN_STAT;

#define DC_SWITCH_ON        0x01
#define GUN1_ASSIT_PWN_ON   0x02
#define GUN1_OUTPUT_ON      0x04
#define GUN2_ASSIT_PWN_ON   0x08
#define GUN2_OUTPUT_ON      0x10
#pragma pack()

// 充电计费模式
typedef enum {
    // 无效充电模式，默认值
    BILLING_MODE_INVALID    =0x00,
    // 自动充电
    BILLING_MODE_AS_AUTO    =0x01,
    // 按金额充电
    BILLING_MODE_AS_MONEY   =0x02,
    // 按时间充电
    BILLING_MODE_AS_TIME    =0x03,
    // 按容量充电
    BILLING_MODE_AS_CAP     =0x04,
    // 自由充电，不需要刷卡
    BILLING_MODE_AS_FREE    =0x05
}BILLING_MODE;

// 充电任务 状态
typedef enum {
    // 无效任务状态
    CHARGE_STAT_INVALID             =0x00,
    // 空闲等待,界面停止于充电模式选择界面
    CHARGE_STAT_TRIGER_PEDDING      =0x01,
    // 刷卡准备, 于自动模式下已经刷卡，或者手动选择了充电计费模式
    //CHARGE_STAT_PREPARE      =0x02,
    // 充电确认, 刷卡完成，进行充电确认等待
    CHARGE_STAT_CONFIRM_PEDDING     =0x03,
    // BMS 连接等待, 已经确认充电
    CHARGE_STAT_WAIT_BMS            =0x04,
    // BMS 已经连接，我收拾别完成，进行充电参数配置
    CHARGE_STAT_READY               =0x05,
    // 充电阶段
    CHARGE_STAT_CHARGING            =0x06,
    // 充电结束阶段, 断开接触器
    CHARGE_STAT_DONE                =0x07,
    // 计费阶段, 等待刷卡付费阶段
    CHARGE_STAT_BILLING_PEDDING     =0x08,
    // 充电任务退出阶段，日志记录，清理
    CHARGE_STAT_EXIT                =0x09,
    // 充电任务被终止
    CHARGE_STAT_ABORT               =0x0A,
    // 充电异常退出
    CHARGE_STAT_EXCEPTION           =0xFF
}CHARGE_TASK_STAT;

// 充电阶段定义
typedef enum {
    // 充电状态无效，可能是在初始化或者析构
    CHARGE_STAGE_INVALID      =0x00,
    // 充电握手
    CHARGE_STAGE_HANDSHACKING =0x01,
    // 充电辨识
    CHARGE_STAGE_IDENTIFICATION =0x02,
    // 充电配置
    CHARGE_STAGE_CONFIGURE    =0x03,
    // 充电
    CHARGE_STAGE_CHARGING     =0x04,
    // 充电结束
    CHARGE_STAGE_DONE         =0x05,
    // 任意阶段
    CHARGE_STAGE_ANY          =0x06,
    // 超时
    CHARGE_STAGE_TIMEOUT      =0x08
}CHARGE_STAGE;

// 充电阶段定义
typedef enum {
    // 充电状态无效，可能是在初始化或者析构
    CHARGE_STAGE_ERR_INVALID      =0x00,
    // 充电握手
    CHARGE_STAGE_ERR_HANDSHACKING =0x10,
    // 充电辨识
    CHARGE_STAGE_ERR_IDENTIFICATION =0x20,
    // 充电配置
    CHARGE_STAGE_ERR_CONFIGURE    =0x30,
    // 充电
    CHARGE_STAGE_ERR_CHARGING     =0x40,
    // 充电结束
    CHARGE_STAGE_ERR_DONE         =0x50,
    // 任意阶段
    CHARGE_STAGE_ERR_ANY          =0x60,
    // 超时
    CHARGE_STAGE_ERR_TIMEOUT      =0x08
}CHARGE_ERR_STAGE;

// 充电阶段变更事件
typedef enum {
    // 充电状态变为无效状态
    STAGE_CHANGE_2_INVALID     = 0x00,
    // 充电状态变为握手阶段
    STAGE_CHANGE_2_HANDSHAKING = 0x01,
    // 充电状态变为配置阶段
    STAGE_CHANGE_2_CONFIGURE   = 0x02,
    // 充电阶段变为充电阶段
    STAGE_CHANGE_2_CHARGING    = 0x03,
    // 充电阶段变为充电完成阶段
    STAGE_CHANGE_2_DONE        = 0x04,
    // 充电阶段变为充电正常退出阶段
    STAGE_CHANGE_2_EXIT        = 0x05,
    // 充电阶段变为中止充电推出阶段
    STAGE_CHANGE_2_ABORT       = 0x06
}CHARGE_STAGE_CHANGE_EVENT;
// 充电阶段改变事件处理过程
void on_charge_stage_change(CHARGE_STAGE_CHANGE_EVENT evt,
                            CHARGE_STAGE pre,
                            struct charge_task *thiz);

// BMS CAN通信状态定义
typedef enum {
    // 无效模式
    CAN_INVALID               =0x00,
    // 普通模式
    CAN_NORMAL                =0x01,
    // 连接管理读模式, 多数据包读
    CAN_TP_RD                 =0x02,
    // 连接管理写模式，多数据包写
    CAN_TP_WR                 =0x04
}CAN_BMS_STATUS;

// CAN 传输链接状态
typedef enum {
    // 链接请求发送
    CAN_TP_RTS = 0x10,
    // 链接准备发送
    CAN_TP_CTS = 0x20,
    // 数据发送中
    CAN_TP_TX  = 0x30,
    // 数据接收中
    CAN_TP_RX  = 0x40,
    // 链接发送完成
    CAN_TP_ACK = 0x50,
    // 链接发送中止
    CAN_TP_ABRT= 0x60
}CAN_TP_STATUS;

struct can_pack_generator;
// 通信报文生成依据
struct can_pack_generator {
    // 所属阶段
    CHARGE_STAGE stage;
    // 生成PGN
    unsigned int pgn;
    // 数据包优先级
    unsigned int prioriy;
    // 数据包长度
    unsigned int datalen;
    // 数据包发送周期
    unsigned int period;
    // 心跳计数
    unsigned int heartbeat;
    // 数据包名称
    const char *mnemonic;
    // 数据包静默时间，只针对接受数据包有效
    unsigned int can_silence;
    // 接受数据包容忍的沉默时常，针对接收数据有效
    unsigned int can_tolerate_silence;
    // 计数器，接收或发送的次数
    unsigned int can_counter;
};

// 充电作业状态
typedef enum {
    // 作业状态为空
    JOB_IDLE = 0x00,
    // 作业状态正在设置
    JOB_SETTING,
    // 作业就绪等待
    JOB_STANDBY,
    // 作业正在执行
    JOB_WORKING,
    // 作业完成，正在进行清理工作
    JOB_DONE
}JOBSTATUS;

/*
 * 充电作业描述，充电管理的最小单位
 */
struct charge_job {
    //  作业状态
    JOBSTATUS job_status;
    // 作业序号
    unsigned int job_sn;
    // 作业充电枪
    CHARGE_GUN_SN job_gun_sn;

};

/*
 * 充电任务描述
 */
struct charge_task {
    // 充电任务编号，唯一，创建充电任务的那一刻
    time_t charge_sn;
    // 充电起始时戳， 闭合充电开关的那一刻
    time_t charge_begin_timestamp;
    // 充电结束时戳， 断开充电开关的那一刻
    time_t charge_stop_timestamp;
    // 转为实施状态时的时戳, 执行实施函数的那一刻
    time_t charge_implemention_timestamp;
    // BMS握手成功的时戳, 接收到第一次BRM的时刻
    time_t charge_bms_establish_timestamp;

    // 任务状态
    CHARGE_TASK_STAT charge_task_stat;
    // 充电任务所处阶段
    CHARGE_STAGE bms_stage;
    CHARGE_ERR_STAGE bms_err_stage;

    // 系统信号, 最多支持64 * 8 种信号标记
    // 前面 16 * 8 = 128 个信号是系统内部使用信号标记
    // 后面 的为遥信 信号定义 @ enum ONTOM_FLAG_SINGLE
    volatile unsigned char single[64];

    // 充电计费
    struct {
        // 计费方式
        BILLING_MODE mode;
        union {
            // 设定充电金额, 0 - 9999
            double set_money;
            // 设定充电时长, 0 - 600
            unsigned int set_time;
            // 设定充电目标容量, 0 - 100
            unsigned int set_cap;
        }option;
    }charge_billing;

    // 充电工作列表
    struct charge_job jobs[CONFIG_SUPPORT_CHARGE_JOBS];
    // 工作列表中的工作个数
    unsigned int nr_jobs;
    // 当前进行的充电工作
    struct charge_job *this_job;

    /* 当前正在使用的充电枪编号
     * 由于目前系统配置为只能一把枪独占式充电，因此必须指明枪的编号
     */
    CHARGE_GUN_SN can_charge_gun_sn;
    // CAN BMS 通信所处状态
    CAN_BMS_STATUS can_bms_status;

    // CAN 通信输入缓冲区
    unsigned char can_buff_in[CAN_BUFF_SIZE];
    volatile unsigned int can_buff_in_nr;
    unsigned int can_packet_pgn;
    // CAN 通信输出缓冲区
    unsigned char can_buff_out[CAN_BUFF_SIZE];
    volatile unsigned int can_buff_out_nr;
    // 标识CAN写缓冲是否满，若缓冲区满，则CAN线程向外写出数据, 写完后置0
    volatile unsigned int can_buff_out_magic;

    // 连接管理时的输出数据包
    unsigned char can_tp_buff_tx[8];
    // 连接管理时的输出数据包大小
    unsigned int can_tp_buff_nr;

    // 连接管理的传输控制参数
    struct can_tp_param can_tp_param;

    // 连接管理超时控制器
    struct Hachiko_food can_tp_bomb;
    struct Hachiko_CNA_TP_private can_tp_private;
    // CAN数据包心跳
    struct Hachiko_food can_heart_beat;

    //充电机握手
    struct pgn9728_CHM charger_handshake;
    //BMS握手
    struct pgn9984_BHM bms_handshake;
    //充电机基本信息
    struct pgn256_CRM charger_info;
    // 车辆基本信息
    struct pgn512_BRM  vehicle_info;
    // BMS充电配置信息
    struct pgn1536_BCP bms_config_info;
    // 充电机时间同步
    struct pgn1792_CTS charger_cts;
    // 充电机最大输出能力参数
    struct pgn2048_CML charger_cml;
    //BMS 准备就绪
    struct pgn2304_BRO bms_bro;
    // 充电机准备就绪
    struct pgn2560_CRO charger_cro;
    // BMS电池充电需求
    struct pgn4096_BCL bms_bcl;
    // BMS电池充电总状态
    struct pgn4352_BCS bms_bcs;
    // 充电机充电状态
    struct pgn4608_CCS charger_ccs;
    // BMS 动力蓄电池状态信息
    struct pgn4864_BSM bms_bsm;
    // BMS BMS中止报文
    struct pgn6400_BST bms_bst;
    // 充电机中止报文
    struct pgn6656_CST charger_cst;
    // BMS 统计数据报文
    struct pgn7168_BSD bms_bsd;
    // 充电机统计数据
    struct pgn7424_CSD charger_csd;
    // BMS 错误报文
    struct pgn7680_BEM bms_bem;
    // 充电机错误报文
    struct pgn7936_CEM charger_cem;
};

//struct charge_task tom;
//struct charge_task *task = &tom;



/* 系统信号定义
 * 前 128个信号为系统标记
 */
typedef enum {
    // CAN 数据包发送标记, 当需要发送对应的PGN数据包时，该位被置1，发送完成后清零
    F_TX_PGN256         = 0x0000,
    F_TX_PGN1792,
    F_TX_PGN2048,
    F_TX_PGN2560,
    F_TX_PGM4608,
    F_TX_PGN6656,
    F_TX_PGN7424,
    F_TX_PGN7936,

    //{{{ 状态标记
    // BMS 已经识别
    F_BMS_RECOGNIZED,
    // 车辆已经识别数据包
    F_VEHICLE_RECOGNIZED,
    F_CHARGER_CTS,//收到充电机时间同步
    F_CHARGER_CML,//收到充电机最大输出参数
    // BMS准备充电报文
    F_BMS_READY,
    // 收到充电机充电准备完成
    F_CHARGER_READY,
    F_BMS_BCL,// BMS电池充电需求标识
    F_BMS_BCS,// BMS电池充电总状态标识
    F_CHARGER_CCS,//收到充电机充电状态
    F_BMS_BST,// BMS终止充电报文
    F_CHARGER_CST,// 充电机终止充电报文
    // 电池充电总状态报文超时
    F_BMS_BCS_TIMEOUT,
    F_BMS_STOP,// BMS 判断充电结束
    F_CHARGER_STOP,// 充电机 判断充电结束

    // 系统人为条件可以充电
    F_MANUAL_CHARGE_ALLOW, // 人为禁止充电
    // 系统硬件条件可以充电
    F_SYSTEM_CHARGE_ALLOW, // 系统无故障
    // 直流输出总开关闭合状态
    F_DC_OUTPUT_SWITCH_STATUS,
    // 1#枪物理连接完成状态
    F_GUN_1_PHY_CONN_STATUS,
    // 1#枪输出开关状态
    F_GUN_1_OUTPUT_SWITCH_STATUS,
    // 1#枪辅助电源开关状态
    F_GUN_1_ASSIT_PWN_SWITCH_STATUS,
    // 2#枪物理连接完成状态
    F_GUN_2_PHY_CONN_STATUS,
    // 2#枪输出开关状态
    F_GUN_2_OUTPUT_SWITCH_STATUS,
    // 2#枪辅助电源开关状态
    F_GUN_2_ASSIT_PWN_SWITCH_STATUS,
    // }}}

    // {{ 输出控制命令状态
    // 直流输出总开关打开
    CMD_DC_OUTPUT_SWITCH_ON  = 0x0080,
    // 1#枪辅助电源打开
    CMD_GUN_1_ASSIT_PWN_ON,
    // 1#枪输出开关打开
    CMD_GUN_1_OUTPUT_ON,
    // 2#枪辅助电源打开
    CMD_GUN_2_ASSIT_PWN_ON,
    // 2#枪输出开关打开
    CMD_GUN_2_OUTPUT_ON,
    // }}

    // {{{ 故障标记
    // 系统遥信量
    S_ERROR             = 0x0100,
    // 交流输入停电故障
    S_AC_INPUT_DOWN,
    // 辅助电源故障
    S_ASSIT_POWER_DOWN,
    // 充电屏监控通信中断
    S_CHARGER_COMM_DOWN,
    // 充电屏监控次要要信1通信中断
    S_CHARGER_YX_1_COMM_DOWN,
    // 充电屏监控次要要信2通信终端
    S_CHARGER_YX_2_COMM_DOWN,
    // 充电模块通信故障
    S_CHARGE_MODULE_COMM_DOWN,
    // 采样单元通信中断
    S_MEASURE_COMM_DOWN,
    // BMS通信故障
    S_BMS_COMM_DOWN,
    // 充电模块故障
    S_CHARGE_MODULE_DOWN,
    // 1# 充电枪物理连接故障
    S_GUN_1_PYH_CONN_DOWN,
    // 2# 充电枪物理连接故障
    S_GUN_2_PYH_CONN_DOWN,
    // 绝缘故障
    S_INSTITUDE_ERR,
    // 母线过压
    S_BUS_V_HI,
    // 母线欠压
    S_BUS_V_LO,
    // 母线短路
    S_BUS_SHORTED,
    // 电池过压
    S_BAT_V_HI,
    // 电池欠压
    S_BAT_V_LO,
    // 电池短路
    S_BAT_SHORTED,
    // 电池反接故障
    S_BAT_REVERT_CONN,
    // 电池过流
    S_BAT_I_HI,
    // 充电桩温度过高
    S_CHARGE_BOX_TEMP_HI,
    // 充电桩温度过低
    S_CHARGE_BOX_TEMP_LO,
    // 充电桩湿度过高
    S_CHARGE_BOX_WET_HI,
    // 充电桩湿度过低
    S_CHARGE_BOX_WET_LO,
    // 总输出熔断器熔断
    S_DC_RDQ_BREAK,
    // 总输出开关跳闸
    S_DC_SW_BREAK,
    // 1#枪输出开关跳闸
    S_GUN_1_SW_BREAK,
    // 2#枪输出开关跳闸
    S_GUN_2_SW_BREAK,
    // 防雷器故障
    S_FANGLEIQI_BREAK,
    // 故障截至标记
    S_ERR_END,

    // 输出电压过高
    S_V_OUT_HIGH,
    // 输出电压过低
    S_V_OUT_LOW,
    // 输出电流过高
    S_I_OUT_HIGH,
    // 输出电流过低
    S_I_OUT_LOW
    // }}}
}ONTOM_FLAG_SINGLE;


static inline void bit_set(struct charge_task *tsk, ONTOM_FLAG_SINGLE single)
{
    volatile unsigned char *byte = tsk->single;

    byte += single / 8;
    * byte |= (1 << (single % 8 ));
}
// 位清除
static inline void bit_clr(struct charge_task *tsk, ONTOM_FLAG_SINGLE single)
{
    volatile unsigned char *byte = tsk->single;

    byte += single / 8;
    * byte &= (~(1 << (single % 8 )));
}
// 位读取
static inline int bit_read(struct charge_task *tsk, ONTOM_FLAG_SINGLE single)
{
    volatile unsigned char *byte = tsk->single;

    byte += single / 8;

    return (* byte & (1 << (single % 8 ))) ? 1 : 0;
}

//1字节crc16计算
static inline void calc_crc16(unsigned short *crc, unsigned short  crcbuf)
{
     unsigned char  i,TT;

    *crc=*crc^crcbuf;
    for(i=0;i<8;i++)
    {
        TT=*crc&1;
        *crc=*crc>>1;
        *crc=*crc&0x7fff;
        if (TT==1)
            (*crc)=(*crc)^0xa001;
        *crc=*crc&0xffff;
    }
}

//多字节CRC16计算
static inline unsigned short load_crc(unsigned short cnt, char *dat)
{
     unsigned short i;
     unsigned short crc=0xffff;
    for(i=0;i<cnt&&i<0xff;i++)
    {
        calc_crc16(&crc,dat[i]);
    }
    return crc;
}

// 大小端转换
static inline unsigned short swap_hi_lo_bytes(unsigned short b)
{
    unsigned char h = b >> 8, l = b & 0xFF;
    return (l << 8 | h);
}
#define b2l swap_hi_lo_bytes
#define l2b swap_hi_lo_bytes

#endif /*_CHARGE_INCLUDED_H_*/
