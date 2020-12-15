#pragma once
#include <QObject>
#include <QFileInfo>
#include <fstream>
#include <QDataStream>
#include "socket.h"
class transmit : public QObject
{
	Q_OBJECT

public:
	explicit transmit(QObject *parent = nullptr);

signals:
	void finished();

public slots:
	void on_run(QString recvIp, unsigned int recvPort);
};