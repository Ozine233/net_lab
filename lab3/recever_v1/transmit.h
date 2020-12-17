#pragma once
#include <QObject>
#include <QFileInfo>
#include <fstream>
#include <QDataStream>
#include "socket.h"
#include "eruptWrite.h"
#include <QThread>

class transmit : public QObject
{
	Q_OBJECT

public:
	explicit transmit(QObject *parent = nullptr);
	~transmit();

signals:
	void finished();
	void startWrite(QString filename);

public slots:
	void on_run1(QString recvIp, unsigned int recvPort);
	void on_run2(QString recvIp, unsigned int recvPort);
private:
	QThread m_eruptWrite_thread;
	eruptWrite *m_eruptWrite;
};