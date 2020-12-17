#include "recever_v1.h"

recever_v1::recever_v1(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	net_init();
	// 初始化传送线程 并启动
	m_transmit = new transmit();
	m_transmit->moveToThread(&m_transmit_threa);
	m_transmit_threa.start();

	// 连接信号槽
	connect(this, &recever_v1::startRunning1, m_transmit, &transmit::on_run1);
	connect(this, &recever_v1::startRunning2, m_transmit, &transmit::on_run2);

	connect(&m_transmit_threa, &QThread::finished, m_transmit, &QObject::deleteLater);

	connect(m_transmit, &transmit::finished, this, &recever_v1::on_finished);
}

recever_v1::~recever_v1()
{
	m_transmit_threa.quit();
	m_transmit_threa.wait();
}

void recever_v1::on_finished()
{
	QMessageBox::information(NULL, tr("Info"), tr("Transmit Finished "));
	ui.waitButton->setEnabled(true);
	ui.windowButton->setEnabled(true);
}

void recever_v1::startWait()
{
	emit startRunning1(ip, port);
}

void recever_v1::startWindow()
{
	emit startRunning2(ip, port);
}

void recever_v1::on_waitButton_clicked()
{
	ui.waitButton->setDisabled(true);
	ui.windowButton->setDisabled(true);

	ip = ui.ip->text();
	port = ui.port->text().toShort();

	startWait();
}

void recever_v1::on_windowButton_clicked()
{
	ui.waitButton->setDisabled(true);
	ui.windowButton->setDisabled(true);

	ip = ui.ip->text();
	port = ui.port->text().toShort();

	startWindow();
}