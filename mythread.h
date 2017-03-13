#ifndef MYTHREAD_H
#define MYTHREAD_H

#include  <QThread>

class mythread : public QThread
{
    Q_OBJECT
public:
    explicit mythread(QObject *parent = 0);
    //bool stop ;
    void stop();
    void run();
signals:

public slots:
};

#endif // MYTHREAD_H
