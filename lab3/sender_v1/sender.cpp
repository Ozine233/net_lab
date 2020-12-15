#include "sender.h"
#include "binaryIO.h"
#include <QRegExpValidator>

sender::sender(QWidget *parent)
	: QWidget(parent) {
	// 建立界面
	ui.setupUi(this);
	ui.progressBar->setRange(0, 8000);
	ui.progressBar->setValue(0);
	// 初始化传送线程 并启动
	m_transmit = new transmit();
	m_transmit->moveToThread(&m_transmit_threa);
	m_transmit_threa.start();

	// 连接信号槽
	connect(this, &sender::startRunningV1, m_transmit, &transmit::on_runV1);
	connect(this, &sender::startRunningV2, m_transmit, &transmit::on_runV2);
	connect(this, &sender::startRunningV3, m_transmit, &transmit::on_runV3);

	connect(&m_transmit_threa, &QThread::finished, m_transmit, &QObject::deleteLater);

	connect(m_transmit, &transmit::infoReady, this, &sender::on_receivInfo);
	connect(m_transmit, &transmit::finished, this, &sender::on_finished);

	// 初始化socket
	if (net_init()) {
		ui.log->append(tr("[INFO] socket start successful!"));
	}
	else {
		ui.log->append(tr("[INFO] socket start fail!"));
	}
}

sender::~sender()
{
	m_transmit_threa.quit();
	m_transmit_threa.wait();
}

void sender::on_fileButton_clicked()
{
	filepath = QFileDialog::getOpenFileName(this, tr("Open transmit file"), "C:", NULL);
	ui.LBfile->setText(filepath);
	fileinfo = QFileInfo(filepath);
	ui.log->append(tr("[INFO] file name : ") + fileinfo.fileName());
	ui.log->append(tr("[INFO] file size : ") + QString::number(fileinfo.size()) + tr(" Byte"));
	ui.progressBar->setRange(0, fileinfo.size());
	ui.progressBar->setValue(0);
}

void sender::disableAllButton()
{
	ui.fileButton->setDisabled(true);
	ui.v1Button->setDisabled(true);
	ui.v2Button->setDisabled(true);
	ui.v3Button->setDisabled(true);
}

void sender::enableAllButton()
{
	ui.fileButton->setEnabled(true);
	ui.v1Button->setEnabled(true);
	ui.v2Button->setEnabled(true);
	ui.v3Button->setEnabled(true);
}

void sender::startV1()
{
	emit startRunningV1(sendIp, sendPort, recvIp, recvPort, filepath);
}

void sender::startV2()
{
	emit startRunningV2(sendIp, sendPort, recvIp, recvPort, filepath);
}

void sender::startV3()
{
	emit startRunningV3(sendIp, sendPort, recvIp, recvPort, filepath);
}

void sender::on_receivInfo(const QString &str, unsigned int ACK)
{
	ui.log->append(str);
	ui.progressBar->setValue(ACK);
}

void sender::on_finished()
{
	ui.log->append(tr("[INFO] Transmit Finished!"));
	enableAllButton();
	ui.progressBar->setValue(fileinfo.size());
}

void sender::on_v1Button_clicked()
{
	disableAllButton();

	sendIp = ui.sendIP->text();
	sendPort = ui.sendPort->text().toShort();
	recvIp = ui.recvIP->text();
	recvPort = ui.recvPort->text().toShort();

	startV1();

	// 	qDebug() << "send ip" << sendIp << endl;
	// 	qDebug() << "send port" << sendPort << endl;
	// 	qDebug() << "recv ip" << recvIp << endl;
	// 	qDebug() << "recv port" << recvPort << endl;
}

void sender::on_v2Button_clicked()
{
	disableAllButton();

	sendIp = ui.sendIP->text();
	sendPort = ui.sendPort->text().toShort();
	recvIp = ui.recvIP->text();
	recvPort = ui.recvPort->text().toShort();

	startV2();
}

void sender::on_v3Button_clicked()
{
}