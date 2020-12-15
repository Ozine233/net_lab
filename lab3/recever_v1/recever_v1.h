#pragma once

#include <QtWidgets/QWidget>
#include <QMessageBox>
#include "ui_recever_v1.h"
#include "socket.h"
#include <QFile>
#include <string>
#include <QDataStream>
#include "transmit.h"
#include <QThread>

class recever_v1 : public QWidget
{
	Q_OBJECT

public:
	recever_v1(QWidget *parent = Q_NULLPTR);
	~recever_v1();
	void start();

signals:
	void startRunning(QString recvIp, unsigned int recvPort);

public slots:
	void on_finished();

private:
	Ui::recever_v1Class ui;
	QString ip;
	PORT port;

	QThread m_transmit_threa;
	transmit *m_transmit;

private slots:
	void on_bindButton_clicked();
};
