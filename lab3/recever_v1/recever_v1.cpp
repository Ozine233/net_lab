#include "recever_v1.h"

recever_v1::recever_v1(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	// 初始化传送线程 并启动
	m_transmit = new transmit();
	m_transmit->moveToThread(&m_transmit_threa);
	m_transmit_threa.start();

	// 连接信号槽
	connect(this, &recever_v1::startRunning, m_transmit, &transmit::on_run);

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
	QApplication* app;
	app->exit(0);
}

void recever_v1::start()
{
	emit startRunning(ip, port);
}

void recever_v1::on_bindButton_clicked()
{
	ui.bindButton->setDisabled(true);
	QApplication* app;
	if (!net_init())
	{
		QMessageBox::information(NULL, tr("Info"), tr("socket open failed! "));
		app->exit(0);
	}
	ip = ui.ip->text();
	port = ui.port->text().toShort();

	start();
}