#pragma once
#include <QObject>
#include <QDataStream>
#include "binaryIO.h"
#include "socket.h"

class transmit : public QObject
{
	Q_OBJECT

public:
	explicit transmit(QObject *parent = nullptr);

signals:
	void infoReady(const QString &str, unsigned int ACK);
	void finished(double time_spend);

public slots:
	void on_runV1(QString sendIp, unsigned int sendPort, QString recvIp, unsigned int recvPort, QString filepath);
	void on_runV2(QString sendIp, unsigned int sendPort, QString recvIp, unsigned int recvPort, QString filepath);
	void on_runV3(QString sendIp, unsigned int sendPort, QString recvIp, unsigned int recvPort, QString filepath);

private:
	binaryIO fileIO;
	QString sendIp;
	unsigned short sendPort;
	QString recvIp;
	unsigned short recvPort;
};