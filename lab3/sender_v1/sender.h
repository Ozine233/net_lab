#pragma once

#include <QtWidgets/QWidget>
#include "ui_sender.h"
#include "qiplineedit.h"
#include "socket.h"
#include <QFileDialog>
#include <QString>
#include <QDebug>

#include <QMessageBox>
#include "transmit.h"
#include "QThread"

class sender : public QWidget
{
	Q_OBJECT

public:
	sender(QWidget *parent = Q_NULLPTR);
	~sender();

	void startV1();
	void startV2();
	void startV3();

signals:
	void startRunningV1(QString sendIp, unsigned int sendPort, QString recvIp, unsigned int recvPort, QString filepath);
	void startRunningV2(QString sendIp, unsigned int sendPort, QString recvIp, unsigned int recvPort, QString filepath);
	void startRunningV3(QString sendIp, unsigned int sendPort, QString recvIp, unsigned int recvPort, QString filepath);

public slots:
	void on_receivInfo(const QString &str, unsigned int ACK);
	void on_finished(double time_spend);

private:
	Ui::senderClass ui;
	binaryIO fileIO;
	QFileInfo fileinfo;
	QString filepath;
	QString sendIp;
	unsigned short sendPort;
	QString recvIp;
	unsigned short recvPort;

	QThread m_transmit_threa;
	transmit *m_transmit;

	void disableAllButton();
	void enableAllButton();

private slots:
	void on_fileButton_clicked();
	void on_v1Button_clicked();
	void on_v2Button_clicked();
	void on_v3Button_clicked();
};
