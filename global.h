#ifndef GLOBAL_H
#define GLOBAL_H

#ifdef WIN32 // for windows
#include <winsock2.h>
#endif // for linux

#include "bms/charge.h"
extern struct charge_task tom;
extern struct charge_task *task;

#include "ControlCAN.h"
extern VCI_CAN_OBJ receive_frame;
extern VCI_CAN_OBJ send_frame;
#endif // GLOBAL_H

extern  int m_cannum;
extern  int m_devtype;
extern  int m_devind;
