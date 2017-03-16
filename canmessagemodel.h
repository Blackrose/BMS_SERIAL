/*
 *  file:		canmessagemodel.h
 *  author:		andy
 *
 *  $Author: andy $
 *  $Date: 2017-03-15 15:50:09 $
 *  $Id: canmessagemodel.h 2017-03-15 13:50:09Z andy $
 */

#ifndef CANMESSAGEMODEL_H_
#define CANMESSAGEMODEL_H_

#include <QAbstractItemModel>
//#include <qcanmessage.h>
#ifdef WIN32 // for windows
#include <winsock2.h>
#endif // for linux
#include "ControlCAN.h"

class CanMessageModel: public QAbstractTableModel
{
public:
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
                autoTrigger(false),
                count(0) {}
    };

    typedef QMap<quint32, QCanMessage> CanMessages;

	enum Type { Receive, Transmit } ;

	CanMessageModel(Type type = Receive, QObject *parent = 0);
	virtual ~CanMessageModel();

#if 1
	void setData(const CanMessages & data);
	void addMessage(const QCanMessage& msg, bool count = true);
	void replaceMessage(const QCanMessage& msg, const QCanMessage& newMsg);
	void deleteMessage(const QCanMessage& msg);
#endif
	void deleteAll();
	void setShowTrigger(bool show);
	bool showTrigger() const {
		return mShowTrigger;
	}


	int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

private:
    Type 		mType;
    CanMessages	mMessages;
    bool mShowTrigger;
};

#endif /* CANMESSAGEMODEL_H_ */
