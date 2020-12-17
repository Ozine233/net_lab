#pragma once
#include <QFile>
#include <QDataStream>
#include <queue>
#include "protocol.h"

class eruptWrite : public QObject
{
	Q_OBJECT

public:
	explicit eruptWrite(QObject *parent = nullptr);
	void push_packet(packet *rpacket);
	bool is_finishsed();
	void set_quit();
	void set_start();

public slots:
	void on_write(QString filename);

private:
	std::queue<packet*> buffer;
	QFile file;
	QDataStream file_out;
	bool quit;
};