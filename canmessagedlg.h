/*
 *  file:		canmessagedlg.h
 *  author:		jrenken
 *
 *  $Rev: 74 $
 *  $Author: andy $
 *  $Date: 2017.3.18 15:50:09 $
 *  $Id: canmessagedlg.h 74 2012-08-30 13:50:09Z andy $
 */
#ifndef CANMESSAGEDLG_H
#define CANMESSAGEDLG_H

#include <QDialog>
#include "ui_canmessagedlg.h"
#include "qcanmessage.h"


class CanMessageDlg : public QDialog
{
    Q_OBJECT

public:
    CanMessageDlg(QWidget *parent = 0);
    CanMessageDlg(const QCanMessage& msg, QWidget *parent = 0);
    ~CanMessageDlg();

    void setMessage(const QCanMessage& msg);
    QCanMessage getMessage() const;

public slots:
	void on_spinBoxDlc_valueChanged(int);


protected:
    void showEvent(QShowEvent *);

private:
    Ui::CanMessageDlgClass ui;

private slots:
	void setExtendedId(bool);
};

#endif // CANMESSAGEDLG_H
