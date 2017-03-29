#include "global.h"


struct charge_task tom;
struct charge_task *task = &tom;

VCI_CAN_OBJ receive_frame;
VCI_CAN_OBJ send_frame;

int m_cannum = 0;
int m_devtype = 4;
int m_devind = 1;
