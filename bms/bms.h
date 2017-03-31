/*
 * BMS 通信数据包定义
 * 版本 V 1.0
 * 遵循标准： GB/T 27930-2011, SAE J1939-21
 */
#ifndef _BMS_PACKAGE_INCLUDED_H_
#define _BMS_PACKAGE_INCLUDED_H_

#ifndef u8
#define u8 unsigned char
#endif
#ifndef u16
#define u16 unsigned short
#endif
#ifndef u32
#define u32 unsigned int
#endif

//add by andy zhao===============================================

/* special address description flags for the CAN_ID */
#define CAN_EFF_FLAG 0x80000000U /* EFF/SFF is set in the MSB */
#define CAN_RTR_FLAG 0x40000000U /* remote transmission request */
#define CAN_ERR_FLAG 0x20000000U /* error message frame */

/* valid bits in CAN ID for frame formats */
#define CAN_SFF_MASK 0x000007FFU /* standard frame format (SFF) */
#define CAN_EFF_MASK 0x1FFFFFFFU /* extended frame format (EFF) */
#define CAN_ERR_MASK 0x1FFFFFFFU /* omit EFF, RTR, ERR flags */


typedef u32 canid_t;

/* CAN payload length and DLC definitions according to ISO 11898-1 */
#define CAN_MAX_DLC 8
#define CAN_MAX_DLEN 8

/**
 * struct can_frame - basic CAN frame structure
 * @can_id:  CAN ID of the frame and CAN_*_FLAG flags, see canid_t definition
 * @can_dlc: frame payload length in byte (0 .. 8) aka data length code
 *           N.B. the DLC field from ISO 11898-1 Chapter 8.4.2.3 has a 1:1
 *           mapping of the 'data length code' to the real payload length
 * @data:    CAN frame payload (up to 8 byte)
 */
struct can_frame {
    canid_t can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
    u8    can_dlc; /* frame payload length in byte (0 .. CAN_MAX_DLEN) */
    u8    data[CAN_MAX_DLEN] __attribute__((aligned(8)));
};


#define RX_WAIT_TIME  100
#define RX_BUFF_SIZE  1
#define TX_BUFF_SIZE  1
#define TEMP_BUFF_SIZE 2048
#define TEMP_BUFFER_SIZE 100

//add end====================================================================


// 充电机地址
#define CAN_ADDR_CHARGER   0x56  // 86
// BMS地址
#define CAN_ADDR_BMS       0xF4  // 244
//#define CAN_RCV_ID_MASK    ((CAN_ADDR_CHARGER<<8)|CAN_ADDR_BMS)
//#define CAN_TX_ID_MASK    ((CAN_ADDR_CHARGER)|CAN_ADDR_BMS<<8)
#define CAN_TX_ID_MASK    ((CAN_ADDR_CHARGER<<8)|CAN_ADDR_BMS)
#define CAN_RCV_ID_MASK    ((CAN_ADDR_CHARGER)|CAN_ADDR_BMS<<8)

#define CAN_TP_CM       0x00EC00 //连接管理
#define CAN_TP_DT       0x00EB00 //数据传送
#define PRI             0x07

#define CAN_TP_CM_ID    ((PRI<< 26)|(CAN_TP_CM<<8)|CAN_TX_ID_MASK|CAN_EFF_FLAG)
#define CAN_TP_DT_ID    ((PRI<< 26)|(CAN_TP_DT<<8)|CAN_TX_ID_MASK|CAN_EFF_FLAG)

#define CAN_TP_CM_RTS_CONTROL 0x10
#define CAN_TP_CM_CTS_CONTROL 0x11
#define CAN_TP_CM_ACK_CONTROL 0x13

#pragma pack(1)
// 握手阶段
//充电机握手报文
struct pgn9728_CHM {
    unsigned char spn2600_charger_version[3];//000101 V1.1
};

//BMS握手报文
struct pgn9984_BHM {
    u16 spn2601_bms_max_vol;
};

// 充电机辨识报文
struct pgn256_CRM {
    // 充电机辨识结果 @ enum recognize_result
    u8 spn2560_recognize;
    // 充电机编号， 范围0-0xFFFFFFFF
    u8 spn2561_charger_sn[4];
    // 充电机/充电站所在区域编码
    unsigned char spn2562_charger_region_code[3];
};
enum recognize_result {
    BMS_RECOGNIZED     =  0xAA, // 充电机能识别BMS
    BMS_NOT_RECOGNIZED =  0x00  // 充电机不能识别BMS
};

// BMS辨识报文
struct pgn512_BRM {
    // BMS 通信协议版本号, 0x000100
    unsigned char spn2565_bms_version[3];
    /* 电池类型
     * 0x01: 铅酸电池
     * 0x02: 镍氢电池
     * 0x03: 磷酸铁锂电池
     * 0x04: 锰酸锂电池
     * 0x05: 钴酸锂电池
     * 0x06: 三元材料电池
     * 0x07: 聚合物锂离子电池
     * 0x08: 钛酸锂电池
     * 0xFF: 其他电池
     */
    u8 spn2566_battery_type;
    // 蓄电池额定容量, 0.1A.H 每位， 0-1000A.H
    u16 spn2567_capacity;
    // 蓄电池额定电压， 0.1V 每位，0-750V
    u16 spn2568_volatage;

    // 以下成员结构为可选结构, 若没有需要使用1填充
    // 电池生产商名称,ASCII 码
    unsigned char spn2569_battery_manufacturer[4];
    // 电池组序列号
    unsigned char spn2570_battery_sn[4];
    // 电池组生产日期
    u8 spn2571_year; // 1985年偏移
    u8 spn2571_month;
    u8 spn2571_day;
    // 电池组充电次数
    unsigned char spn2572_charge_count[3];
    // 电池组产权标识 @ enum battery_property
    u8 spn2573_battery_property;

    u8 spn2574_reserved; // 保留
    // 车辆识别码
    unsigned char spn2575_vin[17];
    // BMS软件版本号
    unsigned char spn2576_bmssoftware_ver[17];
};
enum battery_property {
    BATTERY_LEASE  = 0x00, // 租赁
    BATTERY_OWNER  = 0x01  // 私有
};

// 充电参数配置阶段
// 动力蓄电池充电参数
struct pgn1536_BCP {
    // 单节电池允许最高充电电压, 0.01V 每位， 0-24V
    u16 spn2816_max_charge_volatage_single_battery;
    // 最高允许充电电流, 0.1A每位，-400A偏移 -400A - 0A
    u16 spn2817_max_charge_current;
    // 蓄电池标称总能量, 0.1kW。h每位， 0-1000 kW。h
    u16 spn2818_total_energy;
    // 最高允许充电电压, 0.1V 每位， 0-750V
    u16 spn2819_max_charge_voltage;
    // 最高允许温度，1 度每位，-50度偏移，-50 度 - 200度
    u8 spn2820_max_temprature;
    // 整车蓄电池荷电状态，0.1% 每位，0-100%
    u16 spn2821_soc;
    // 整车蓄电池总电压， 0.1V 每位，0-750V
    u16 spn2822_total_voltage;
};

// 充电机发送时间同步信息
struct pgn1792_CTS {
    // 压缩的BCD码
    // 第6-7字节为年
    u8 spn2823_bcd_sec;
    u8 spn2823_bcd_min;
    u8 spn2823_bcd_hour;
    u8 spn2823_bcd_day;
    u8 spn2823_bcd_mon;
    u8 spn2823_bcd_year_h;
    u8 spn2823_bcd_year_l;
};

// 充电机最大输出能力
struct pgn2048_CML {
    // 最大输出电压, 0.1V 每位， 0-750V
    u16 spn2824_max_output_voltage;
    // 最低输出电压，0.1V 每位，0-750V
    u16 spn2825_min_output_voltage;
    // 最大输出电流，0.1A 每位，-400A偏移，范围-400-0 A
    u16 spn2826_max_output_current;
    // 最小输出电流，0.1A 每位，-400A偏移，范围-400-0 A
    u16 spn2826_min_output_current;//add for GBT 27930-2015
};

// 电池充电准备就绪状态
struct pgn2304_BRO {
    // BMS 充电准备完成 @ enum charger_charge_status
    u8 spn2829_bms_ready_for_charge;
};
enum bms_charge_status {
    BMS_NOT_READY_FOR_CHARGE =  0x00, // 没有准备好
    BMS_READY_FOR_CHARGE     =  0xAA, // 已准备好
    BMS_INVALID              =  0xFF // 无效
};

// 充电机输出准备就绪状态
struct pgn2560_CRO {
    // 充电机充电准备完成 @ enum charger_charge_status
    u8 spn2830_charger_ready_for_charge;
};
enum charger_charge_status {
    CHARGER_NOT_READY_FOR_CHARGE = 0x00, // 没有准备好
    CHARGER_READY_FOR_CHARGE     = 0xAA, // 已准备好
    CHARGER_INVALID              = 0xFF // 无效
};

// 充电阶段
// 电池充电需求
struct pgn4096_BCL {
    // 充电电压需求，0.1V 每位，0-750V
    u16 spn3072_need_voltage;
    // 充电电流需求，0.1A 每位，-400V偏移，-400A-0A
    u16 spn3073_need_current;
    // 充电模式 @ enum charge_mode
    u8  spn3074_charge_mode;
};
enum charge_mode {
    CHARGE_WITH_CONST_VOLTAGE  = 0x01,  // 恒流充电
     CHARGE_WITH_CONST_CURRENT = 0x02  // 恒压充电
};

// 电池充电总状态
struct pgn4352_BCS {
    // BMS 的充电电压测量值，0.1V 每位，0V偏移， 0-750V
    u16 spn3075_charge_voltage;
    // BMS的充电电流测量值，0.1A 每位， -400A偏移， -400A-0A
    u16 spn3076_charge_current;
    // 最高单体动力蓄电池电压及其组号，1-12位： 最高单体电池电压， 0.01V 每位， 0-24V
    // 13-16： 最高电压电池所在组号， 1/位， 1-16
    u16 spn3077_max_v_g_number;
    // 当前荷电容量, 1% 每位， 0-100%
    u8  spn3078_soc;
    // 剩余充电时间, 1Min 每位 0-600min
    u16 spn3079_need_time;
};

// 充电机充电状态
struct pgn4608_CCS {
    // 充电机输出电压，0.1V/位， 0V偏移量，0-750V
    u16 spn3081_output_voltage;
    // 充电机输出电流，0.1A/位，-400A偏移，-400A-0A
    u16 spn3082_outpu_current;
    // 充电持续时间，1min/位，0偏移，0-600min
    u16 spn3083_charge_time;
    // 充电允许 @ enum charger_status
    u8  spn3929_charger_status;//add for GBT 27930-2015
};

enum charger_status {
    CHARGER_ALLOW = 0x00, // 充电允许
    CHARGER_PAUSE = 0x01 // 充电暂停
};

// 动力蓄电池状态信息
struct pgn4864_BSM {
    // 最高单体电压的蓄电池编号
    u8 sn_of_max_voltage_battery;
    // 最高蓄电池温度
    u8 max_temperature_of_battery;
    // 最高温度采样点编号
    u8 sn_of_max_temperature_point;
    // 最低蓄电池温度
    u8 min_temperature_of_battery;
    // 最低温度采样点编号
    u8 sn_of_min_temperature_point;
    // 遥信 @ enum pgn4846_remote_single
    u16 remote_single;
};
enum pgn4846_remote_single {
    // 单体蓄电池电压正常
    SINGLE_BATTERY_VOLTAGE_NORMAL = 0x00000000,
    // 单体蓄电池电压过高
    SINGLE_BATTERY_VOLTAGE_HIGH   = 0x00000001,
    // 单体学电池电压过低
    SINGLE_BATTERY_VOLTAGE_LOW    = 0x00000002,

    // 蓄电池组荷电状态正常
    BATTERY_SOC_NORMAL            = 0x00000000,
    // 蓄电池组荷电状态过高
    BATTERY_SOC_HIGH              = 0x00000004,
    // 蓄电池组荷电状态过低
    BATTERY_SOC_LOW               = 0x00000008,

    // 充电电流正常
    BATTERY_CHARGE_CURRENT_NORMAL = 0x00000000,
    // 充电电流过高
    BATTERY_CHARGE_CURRENT_HIGH   = 0x00000010,
    // 充电电流过低
    BATTERY_CHARGE_CURRENT_LOW    = 0x00000020,

    // 电池温度正常
    BATTERY_TEMPRATUEE_NORAML     = 0x00000000,
    // 电池温度过高
    BATTERY_TEMPRATURE_HIGH       = 0x00000040,
    // 电池温度过低
    BATTERY_TEMPRATURE_LOW        = 0x00000080,

    // 电池绝缘状态正常
    INSULATION_NORMAL             = 0x00000000,
    // 电池有绝缘故障
    INSULATION_FAULT              = 0x00000100,
    // 绝缘信号不可信
    INSULATION_UNRELIABLE         = 0x00000200,

    // 冲电连接器状态正常
    CONNECTOR_STATUS_NORMAL       = 0x00000000,
    // 充电连接器不正常
    CONNECTOR_STATUS_FAULT        = 0x00000400,
    // 冲电连接器状态不可信
    CONNECTOR_STATUS_UNRELIABLE   = 0x00000800,

    // 允许充电
    CHARGE_ALLOWED                = 0x00000000,
    // 禁止充电
    CHARGE_FORBIDEN               = 0x00001000
};

// 单体动力蓄电池电压
struct pgn5376_BMV {
    // bit[0:11]: 电压， 0.01V 每位, 0V偏移，范围 0- 24V
    // bit[12:15]: 蓄电池编号，1 / 每位， 范围 1-16
    u16 bat_v[256];
};

// 动力蓄电池温度
struct pgn5632_BMT {
    // 1 度每位， -50度偏移， 范围 -50 - 200
    u16 bat_tmp[128];
};

// 动力蓄电池预留报文
struct pgn5888_BSP {
    u16 reseved[16];
};

// BMS终止充电
struct pgn6400_BST {
    // BMS 中止充电原因 @ enum REASON_PGN6400
    u8 reason;//spn3511
    // BMS 中止充电故障原因 enum ERROR_PGN6400
    u16 error;//spn3512
    // BMS 中止充电错误原因 enum FAULT_PGN6400
    u8 fault;//spn3513
};
enum REASON_PGN6400 {
    // 达到所需SOC值 bit[0:1]
    REASON_UN_REACH_SOC_VAL  = 0x00,
    REASON_REACH_SOC_VAL     = 0x01,
    REASON_SOC_UNRELIABLE        = 0x02,

    // 达到总电压设定值 bit[2:3]
    REASON_UN_REACH_VOL_VAL  = 0x00,
    REASON_REACH_VOL_VAL     = 0x04,
    REASON_VOL_UNRELIABLE        = 0x08,
    // 充电机主动中止 bit[4:5]
    REASON_UN_REACH_SINGLE_BAT_VOL = 0x00,
    REASON_REACH_SINGLE_BAT_VOL    = 0x10,
    REASON_SINGLE_VOL_UNRELIABLE   = 0x20
};
enum ERROR_PGN6400 {
    // 绝缘故障 bit[0:1]
    ERROR_INSULATION_NORMAL  = 0x00,
    ERROR_INSULATION         = 0x01,
    ERROR_INSULATION_UNRELIABLE    = 0x02,
    // 输出连接器过温 bit[2:3]
    ERROR_OUTPUTCONNECTOR_NORMAL = 0x00,
    ERROR_OUTPUTCONNECTOR        = 0x04,
    ERROR_OUTPUTCONNECTOR_UNRELIABLE = 0x08,
    // BMS元件输出连接器过温 bit[4:5]
    ERROR_BMS_OUTPUTCONNECTOR_NORMAL = 0x00,
    ERROR_BMS_OUTPUTCONNECTOR        = 0x10,
    ERROR_BMS_OUTPUTCONNECTOR_UNRELIABLE = 0x20,
    // 充电连接器故障 bit[6:7]
    ERROR_CHARGINGCONNECTOR_NORMAL = 0x00,
    ERROR_CHARGINGCONNECTOR        = 0x40,
    ERROR_CHARGINGCONNECTOR_UNRELIABLE = 0x80,
    // 电池组过温故障 bit[8:9]
    ERROR_BATTERY_TEMP_NORMAL = 0x00,
    ERROR_BATTERY_TEMP        = 0x100,
    ERROR_BATTERY_TEMP_UNRELIABLE = 0x200,
    // 高压继电器故障 bit[10:11]
    ERROR_HIGH_VOL_RELAY_NORMAL = 0x00,
    ERROR_HIGH_VOL_RELAY        = 0x400,
    ERROR_HIGH_VOL_RELAY_UNRELIABLE = 0x800,
    //检测点2电压检测故障 bit[12:13]
    ERROR_POINT2_VOL_DETECTED_NORMAL = 0x00,
    ERROR_POINT2_VOL_DETECTED = 0x1000,
    ERROR_POINT2_VOL_DETECTED_UNRELIABLE = 0x2000,
    //其他故障 bit[14:15]
    ERROR_OTHER_NORMAL = 0x00,
    ERROR_OTHER = 0x4000,
    ERROR_OTHER_UNRELIABLE = 0x8000
};
enum FAULT_PGN6400 {
    //电流过大 bit[0:1]
    FAULT_CURRENT_NORMAL = 0x00,
    FAULT_CURRENT_EXCEEDS_DEMAND = 0x01,
    FAULT_CURRENT_UNRELIABLE     = 0x02,
    //电压异常 bit[2:3]
    FAULT_VOL_NORMAL        = 0x00,
    FAULT_VOL_UN_NORMAL     = 0x04,
    FAULT_VOL_UNRELIABLE    = 0x08
};

// 充电机终止充电
struct pgn6656 {
    // charger 中止充电原因 @ enum REASON_PGN6656
    u8 spn3521_reason;
    // charger 中止充电故障原因 enum ERROR_PGN6656
    u16 spn3521_error;
    // charger 中止充电错误原因 enum FAULT_PGN6656
    u8 spn3521_fault;
};

enum REASON_PGN6656 {
    // 达到charger设定条件中止 bit[0:1]
    REASON_CHARGER_NORMAL  = 0x00,
    REASON_REACH_CHARGER_STOP     = 0x01,
    REASON_CHARGER_UNRELIABLE     = 0x02,
    // 人工中止 bit[2:3]
    REASON_CHARGER_MANUAL_NORMAL  = 0x00,
    REASON_CHARGER_MANUAL_STOP    = 0x04,
    REASON_CHARGER_MANUAL_UNRELIABLE = 0x08,
    // 故障中止 bit[4:5]
    REASON_CHARGER_NO_ERROR       = 0x00,
    REASON_CHARGER_ERROR_STOP     = 0x10,
    REASON_CHARGER_ERROR_UNRELIABLE   = 0x20,
    // BMS主动中止 bit[6:7]
    REASON_BMS_NORMAL   = 0x00,
    REASON_BMS_STOP     = 0x40,
    REASON_BMS_UNRELIABLE     = 0x80
};
enum ERROR_PGN6656 {
    // charger过温故障 bit[0:1]
    ERROR_CHARGER_TEMP_NORMAL  = 0x00,
    ERROR_CHARGER_TEMP         = 0x01,
    ERROR_CARGER_TEMP_UNRELIABLE    = 0x02,
    // 充电连接器过温 bit[2:3]
    ERROR_CHARGINGCONNECTOR_TEMP_NORMAL = 0x00,
    ERROR_CHARGINGCONNECTOR_TEMP        = 0x04,
    ERROR_CHARGINGCONNECTOR_TEMP_UNRELIABLE = 0x08,
    // 充电机内部过温 bit[4:5]
    ERROR_CHARGER_INTEMP_NORMAL = 0x00,
    ERROR_CHARGER_INTEMP        = 0x10,
    ERROR_CHARGER_INTEMP_UNRELIABLE = 0x20,
    // 所需电量不能传送 bit[6:7]
    ERROR_POWER_TRANSFER_NORMAL = 0x00,
    ERROR_POWER_TRANSFER        = 0x40,
    ERROR_POWER_TRANSFER_UNRELIABLE = 0x80,
    // 充电机急停故障 bit[8:9]
    ERROR_CHARGER_EMERGENCY_STOP_NORMAL = 0x00,
    ERROR_CHARGER_EMERGENCY_STOP        = 0x100,
    ERROR_CHARGER_EMERGENCY_STOP_UNRELIABLE = 0x200,
    // 其他故障 bit[10:11]
    ERROR_OTHERPGN6656_NORMAL = 0x00,
    ERROR_OTHERPGN6656        = 0x400,
    ERROR_OTHERPGN6656_UNRELIABLE = 0x800
};
enum FAULT_PGN6656 {
    //电流过大 bit[0:1]
    FAULT_CURRENT_MATCHING_NORMAL = 0x00,
    FAULT_CURRENT_UN_MATCHING     = 0x01,
    FAULT_CURRENT_PGN6656_UNRELIABLE      = 0x02,
    //电压异常 bit[2:3]
    FAULT_VOL_PGN6656_NORMAL        = 0x00,
    FAULT_VOL_PGN6656_UN_NORMAL     = 0x04,
    FAULT_VOL_PGN6656_UNRELIABLE    = 0x08
};

// 充电结束阶段
// BMS统计数据
struct pgn7168 {
    //中止荷电状态SOC 1% 每位，0%偏移，范围0-100%
    u8 spn3601_stop_soc_status;
    //动力蓄电池单体最低电压 0.01V 每位, 0V偏移，范围 0- 24V
    u16 spn3602_singal_battery_min_vol;
    //动力蓄电池单体最高电压 0.01V 每位, 0V偏移，范围 0- 24V
    u16 spn3603_singal_battery_max_vol;
    //动力蓄电池最低温度 1 度每位， -50度偏移， 范围 -50 - 200
    u8 spn3604_battery_min_temp;
    //动力蓄电池最高温度 1 度每位， -50度偏移， 范围 -50 - 200
    u8 spn3605_battery_max_temp;
};

// 充电机统计数据
struct pgn7424 {
    //累计充电时间, 1Min 每位 0 min偏移 范围 0-600min
    u16 spn3611_total_charge_time;
    //输出能量 0.1kW.h每位， 范围 0-1000 kW。h
    u16 spn3612_output_energy;
    // 充电机编号， 1每位 1偏移量 范围 0-0xFFFFFFFF
    u8 spn3613_charger_sn[4];
};

// 错误报文分类
// BMS 错误报文
struct pgn7680 {
    u8 bem_crm;//@ enum BEM_PGN3901_3902
    u8 bem_ccp;//@ enum BEM_PGN3903_3904
    u8 bem_cst;//@ enum BEM_PGN3905_3906
    u8 bem_csd;//@ enum BEM_PGN3907
};

enum BEM_PGN3901_3902{
    //接收spn2560 ==0x00 辨识报文超时 bit[0:1]
    BEM_00_NORMAL       = 0x00,
    BEM_00_TIMEOUT      = 0x01,
    BEM_00_UNRELIABLE   = 0x02,
    //接收spn2560 ==0xAA 辨识报文超时 bit[2:3]
    BEM_AA_NORMAL       = 0x00,
    BEM_AA_TIMEOUT      = 0x04,
    BEM_AA_UNRELIABLE   = 0x08
};
enum BEM_PGN3903_3904{
    //接收充电机的时间同步和充电机最大输出能力报文超时 bit[0:1]
    BEM_CTS_CML_NORMAL       = 0x00,
    BEM_CTS_CML_TIMEOUT      = 0x01,
    BEM_CTS_CML_UNRELIABLE   = 0x02,
    //接收充电机完成充电准备报文超时 bit[2:3]
    BEM_CRO_NORMAL       = 0x00,
    BEM_CRO_TIMEOUT      = 0x04,
    BEM_CRO_UNRELIABLE   = 0x08
};
enum BEM_PGN3905_3906{
    //接收充电机充电状态报文超时 bit[0:1]
    BEM_CCS_NORMAL       = 0x00,
    BEM_CCS_TIMEOUT      = 0x01,
    BEM_CCS_UNRELIABLE   = 0x02,
    //接收充电机中止充电报文超时 bit[2:3]
    BEM_CST_NORMAL       = 0x00,
    BEM_CST_TIMEOUT      = 0x04,
    BEM_CST_UNRELIABLE   = 0x08
};
enum BEM_PGN3907{
    //接收充电机统计报文超时 bit[0:1]
    BEM_CSD_NORMAL       = 0x00,
    BEM_CSD_TIMEOUT      = 0x01,
    BEM_CSD_UNRELIABLE   = 0x02
};

// 充电机错误报文
struct pgn7936 {
    u8 cem_brm;//@ enum BEM_PGN3921
    u8 cem_bro;//@ enum BEM_PGN3922_3923
    u8 cem_bst;//@ enum BEM_PGN3924_3925_3926
    u8 cem_bsd;//@ enum BEM_PGN3927
};

enum BEM_PGN3921{
    //接收BMS辨识报文超时 bit[0:1]
    BEM_BMS_NORMAL       = 0x00,
    BEM_BMS_TIMEOUT      = 0x01,
    BEM_BMS_UNRELIABLE   = 0x02
};
enum BEM_PGN3922_3923{
    //接收电池充电参数报文超时 bit[0:1]
    BEM_BCP_NORMAL       = 0x00,
    BEM_BCP_TIMEOUT      = 0x01,
    BEM_BCP_UNRELIABLE   = 0x02,
    //接收BMS完成充电准备报文超时 bit[2:3]
    BEM_BRO_NORMAL       = 0x00,
    BEM_BRO_TIMEOUT      = 0x04,
    BEM_BRO_UNRELIABLE   = 0x08
};
enum BEM_PGN3924_3925_3926{
    //接收电池充电总状态报文超时 bit[0:1]
    BEM_BCS_NORMAL       = 0x00,
    BEM_BCS_TIMEOUT      = 0x01,
    BEM_BCS_UNRELIABLE   = 0x02,
    //接收电池充电需求报文超时 bit[2:3]
    BEM_BCL_NORMAL       = 0x00,
    BEM_BCL_TIMEOUT      = 0x04,
    BEM_BCL_UNRELIABLE   = 0x08,
    //接收BMS中止充电报文超时 bit[4:5]
    BEM_BST_NORMAL       = 0x00,
    BEM_BST_TIMEOUT      = 0x10,
    BEM_BST_UNRELIABLE   = 0x20
};
enum BEM_PGN3927{
    //接收BMS统计报文超时 bit[0:1]
    BEM_BSD_NORMAL       = 0x00,
    BEM_BSD_TIMEOUT      = 0x01,
    BEM_BSD_UNRELIABLE   = 0x02
};



#pragma pack()

typedef enum {
    PGN_CHM = 0x002600,
    PGN_BHM = 0x002700,
    PGN_CRM = 0x000100,
    PGN_BRM = 0x000200,
    PGN_BCP = 0x000600,
    PGN_CTS = 0x000700,
    PGN_CML = 0x000800,
    PGN_BRO = 0x000900,
    PGN_CRO = 0x000A00,
    PGN_BCL = 0x001000,
    PGN_BCS = 0x001100,
    PGN_CCS = 0x001200,
    PGN_BSM = 0x001300,
    PGN_BMV = 0x001500,
    PGN_BMT = 0x001600,
    PGN_BSP = 0x001700,
    PGN_BST = 0x001900,
    PGN_CST = 0x001A00,
    PGN_BSD = 0x001C00,
    PGN_CSD = 0x001D00,
    PGN_BEM = 0x001E00,
    PGN_CEM = 0x001F00
}CAN_PGN;

#include "Hachiko.h"

/*
 * 读到一个完整数据包后调用该函数
 */
typedef enum {
    // 无效事件，初始化完成前收到该消息
    EVENT_INVALID    = 0,

    /* 数据包接受完成。系统中存在两个和CAN接口相关的线程，一个
     * 是CAN数据读取线程，还有一个是CAN数据发送线程，这两个线程
     * 对CAN的操作都是同步的，因此，在读取数据包时读线程会被阻塞
     * 因此需要通过该事件来通知系统，已经完成了数据包的读取。
     */
    EVENT_RX_DONE    = 1,

    /* 数据包接收错误。在阻塞读取CAN数据包时，发生数据包读取错误，
     * 会收到该事件，表征数据包有问题，应该作相应处理。
     */
    EVENT_RX_ERROR   = 2,

    /* 数据包发送请求。系统中CAN数据的发送线程是同步的，可以通过
     * 该事件来提醒系统，目前可以进行数据包的发送，若系统有数据包发送
     * 则设定发送数据，交由发送县城进行发送，若没有数据要发送，则返回一个
     * 不需要发送的结果即可。
     */
    EVENT_TX_REQUEST = 3,

    /* 连接管理模式下的请求发送数据包，进行连接数据控制。
     */
    EVENT_TX_TP_RTS = 4,
    /* 连接管理模式下的准备发送数据包，进行连接数据控制。
     */
    EVENT_TX_TP_CTS = 5,
    /* 连接管理模式下的数据发送包，进行数据传递控制。
     */
    EVENT_TX_TP_DT = 6,
    /* 连接管理模式下的接收数据包完成应答，进行连接数据控制。
     */
    EVENT_TX_TP_ACK = 7,
    /* 连接管理模式下的传输中止，进行连接数据控制。
     */
    EVENT_TX_TP_ABRT= 8,

    /* 数据包准备发送。当EVENT_TX_REQUEST返回结果是需要发送时，经发送线程
     * 经发送线程确认后，将会受到该消息，表示发送线程已经准备发送该消息了，此时
     * 可以返回取消发送指令，实现数据包的取消发送。
     */
    EVENT_TX_PRE     = 9,

    /* 数据包发送完成。当确认后的数据包发送完成后，将会受到该消息，表征数据包
     * 已经正确的发送完成。
     */
    EVENT_TX_DONE    = 10,

    /* 数据包发送失败。当确认后的数据包发送失败后，将会受到改小。
     */
    EVENT_TX_FAILS   = 11,

    // CAN 消息函数初始化。当第一次运行函数时将会收到该消息，可重复发送。
    EVENT_CAN_INIT   = 12,

    // CAN 消息复位。再次执行初始化操作。
    EVENT_CAN_RESET  = 13
}EVENT_CAN;

// 事件通知返回/传入参数
typedef enum {
    // 无效
    EVT_RET_INVALID  = 0,
    // 一切OK
    EVT_RET_OK       = 1,
    // 失败了
    EVT_RET_ERR      = 2,
    // 超时
    EVT_RET_TIMEOUT  = 3,
    // 终止发送，EVENT_CAN.EVENT_TX_PRE的可用参数
    EVT_RET_TX_ABORT = 4
}EVT_PARAM;

// CAN 链接临时参数
struct can_tp_param {
    // 传输的数据包PGN
    unsigned int tp_pgn;
    // 即将传输的数据包大小
    unsigned int tp_size;
    // 即将传输的数据包个数
    unsigned int tp_pack_nr;
    // 即将发送的数据包编号
    unsigned int tp_pack_num;
    // 即将发送的数据
    unsigned char tx_buff[TEMP_BUFFER_SIZE];
    // 已经接收的数据字节数
    unsigned int tp_rcv_bytes;
    // 已经接收的数据包个数
    unsigned int tp_rcv_pack_nr;
};

// 事件通知结构
struct event_struct {
    // 事件参数
    EVT_PARAM evt_param;

    union {
        // 发送缓冲区地址， 针对EVENT_TX_REQUEST设置
        unsigned char *tx_buff;
        // 接收缓冲区地址，针对EVENT_RX_DONE设置
        const unsigned char* rx_buff;
    }buff;
    // 缓冲区大小
    unsigned int buff_size;
    // CAN ID
    unsigned int can_id;
    // 缓冲区载荷大小
    unsigned int buff_payload;
};

// BMS通信统计技术结构
struct bms_statistics {
    // 数据包PGN
    CAN_PGN can_pgn;
    // 数据包静默时间，只针对接受数据包有效
    unsigned int can_silence;
    // 接受数据包容忍的沉默时常，针对接收数据有效
    unsigned int can_tolerate_silence;
    // 计数器，接收或发送的次数
    unsigned int can_counter;
};
typedef enum {
    I_CHM = 0,
    I_BHM = 1,
    I_CRM = 2,
    I_BRM = 3,
    I_BCP = 4,
    I_CTS = 5,
    I_CML = 6,
    I_BRO = 7,
    I_CRO = 8,
    I_BCL = 9,
    I_BCS = 10,
    I_CCS = 11,
    I_BSM = 12,
    I_BMV = 13,
    I_BMT = 14,
    I_BSP = 15,
    I_BST = 16,
    I_CST = 17,
    I_BSD = 18,
    I_CSD = 19,
    I_BEM = 20,
    I_CEM = 21
}CAN_PGN_STATISTICS;

int about_packet_transfer_done(struct charge_task *thiz,
                             struct event_struct *param);
int about_packet_reciev_done(struct charge_task *thiz,
                             struct event_struct *param);
//握手阶段
int gen_packet_PGN9984(struct charge_task * thiz,
                       struct event_struct* param);

//辨识阶段
int gen_packet_PGN512(struct charge_task * thiz,
                       struct event_struct* param);

//参数配置阶段
int gen_packet_PGN1536(struct charge_task * thiz,
                        struct event_struct* param);
int gen_packet_PGN2304(struct charge_task * thiz,
                        struct event_struct* param);

//充电阶段
int gen_packet_PGN4096(struct charge_task * thiz,
                        struct event_struct* param);
int gen_packet_PGN4352(struct charge_task * thiz,
                        struct event_struct* param);
int gen_packet_PGN4864(struct charge_task * thiz,
                        struct event_struct* param);
int gen_packet_PGN5376(struct charge_task * thiz,
                        struct event_struct* param);
int gen_packet_PGN5632(struct charge_task * thiz,
                        struct event_struct* param);
int gen_packet_PGN5888(struct charge_task * thiz,
                        struct event_struct* param);
int gen_packet_PGN6400(struct charge_task * thiz,
                        struct event_struct* param);

//充电结束阶段
int gen_packet_PGN7168(struct charge_task * thiz,
                        struct event_struct* param);
//错误报文
int gen_packet_PGN7680(struct charge_task * thiz,
                        struct event_struct* param);


void set_packet_TP_CM_RTS(int png_num,struct event_struct* param);
void set_packet_TP_DT(int png_num,struct event_struct* param);

int set_data_tcu_PGN9984(struct charge_task * thiz);
int set_data_tcu_PGN512(struct charge_task * thiz);
int set_data_tcu_PGN2304(struct charge_task * thiz);


#ifdef BMS_CC_LANG
#define EXTERNC		extern "C"
#else
#define EXTERNC
#endif

EXTERNC int bms_canbus();

#endif /*_BMS_PACKAGE_INCLUDED_H_*/
