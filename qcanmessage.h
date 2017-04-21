/*
 *  file:		canmessage.h
 *  author:		andy
 *
 *  $Rev: 74 $
 *  $Author: andy $
 *  $Date: 2017.3.18 15:50:09 $
 *  $Id: canmessage.h 74 2017.3.18 13:50:09Z andy $
 */

#ifndef CANMESSAGE_H_
#define CANMESSAGE_H_

#include <QMap>
#ifdef WIN32 // for windows
#include <winsock2.h>
#endif // for linux
#include "ControlCAN.h"

//using namespace QtAddOn;

struct QCanMessage {
    VCI_CAN_OBJ	frame;
	qint64		time;
	qint64		sinceLast;
	qint64		period;
	bool		autoTrigger;
	int			count;
	QCanMessage()
		:	time(0),
		 	sinceLast(0),
		 	period(0),
            autoTrigger(true),//false
		 	count(0) {}
};

typedef QMap<quint32, QCanMessage> CanMessages;

#endif /* CANMESSAGE_H_ */
