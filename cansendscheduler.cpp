/*
 * CanSendScheduler.cpp
 *
 *  Created on: 2017.3.18
 *      Author: andy
 */

#include <QTimerEvent>
#include <QDebug>
#include "cansendscheduler.h"

CanSendScheduler::CanSendScheduler(QObject* parent)
	: QObject(parent)
{
}


void CanSendScheduler::addJob(const QCanMessage& msg)
{
	removeJob(msg);
	if (msg.autoTrigger && msg.period > 0) {
		int timer = startTimer(msg.period);
		mJobs.insert(timer, msg);
	}
}

void CanSendScheduler::removeJob(const QCanMessage& msg)
{
	foreach (int key, mJobs.keys()) {
        if (mJobs.value(key).frame.ID == msg.frame.ID) {
			killTimer(key);
			mJobs.remove(key);
		}
	}
}

void CanSendScheduler::clearAll()
{
	foreach (int key, mJobs.keys()) {
		killTimer(key);
	}
	mJobs.clear();
}
void CanSendScheduler::timerEvent(QTimerEvent* event)
{
	QCanMessage msg = mJobs.value(event->timerId());
	emit jobScheduled(msg);
}
