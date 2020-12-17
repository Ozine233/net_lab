#include "transmit.h"
#include <QDebug>
#include <QThread>
#include <QMessageBox>

transmit::transmit(QObject *parent /* = nullptr */)
{
	m_eruptWrite = new eruptWrite();
	m_eruptWrite->moveToThread(&m_eruptWrite_thread);
	m_eruptWrite_thread.start();

	// 连接信号槽
	connect(this, &transmit::startWrite, m_eruptWrite, &eruptWrite::on_write);
}

transmit::~transmit()
{
	m_eruptWrite_thread.quit();
	m_eruptWrite_thread.wait();
}

// 停等机制  接收端
void transmit::on_run1(QString recvIp, unsigned int recvPort)
{
	// 	SOCKET recv_Socket = socket(AF_INET, SOCK_DGRAM, 0);
	//
	// 	// 设置接收端socket地址
	// 	SOCKADDR_IN *addr = get_addr(ip.toStdString(), port);
	//
	// 	bind(recv_Socket, (SOCKADDR*)addr, sizeof(SOCKADDR));
	//
	// 	while (1)
	// 	{
	// 		char recvdata[80] = { 0 };
	// 		packet * mypacket = nullptr;
	// 		int len = sizeof(*addr);
	// 		const char *s2cData = "hello,client,this is the server!\n";
	// 		//recvfrom( SOCKET s, char FAR* buf, int len, int flags,struct sockaddr FAR* from, int FAR* fromlen);
	// 		//s：标识一个已连接套接口的描述字。
	// 		//buf：接收数据缓冲区。
	// 		//len：缓冲区长度。
	// 		//flags：调用操作方式。
	// 		//from：（可选）指针，指向装有源地址的缓冲区。
	// 		//fromlen：（可选）指针，指向from缓冲区长度值。
	// 		//int recvnum=recvfrom(recv_Socket,recvdata,strlen(recvdata),0,(struct sockaddr *)&addr,&len);
	//
	// 		int recvnum = recvfrom(recv_Socket, recvdata, MAX_BUF, 0, (struct sockaddr *)addr, &len);
	// 		//int recvnum = recv_packet(recv_Socket, mypacket, addr, len);
	// 		if (recvnum > 0)
	// 		{
	// 			mypacket = (packet *)recvdata;
	// 			if (check_packet(mypacket))
	// 			{
	// 				QMessageBox::information(NULL, tr("Info"), tr("666666"));
	// 			}
	// 			sendto(recv_Socket, s2cData, strlen(s2cData), 0, (struct sockaddr *)addr, sizeof(*addr));
	// 			qDebug("send to\n%s\n", s2cData);
	// 		}
	// 	}

	SOCKET recv_socket = NULL;
	SOCKADDR_IN *addr = get_addr(recvIp.toStdString(), recvPort);
	recv_socket = get_socket(addr);

	packet * s_packet = make_packet(0, 0, 0, 0, 0, NULL);
	unsigned int ACK = 0;

	m_eruptWrite->set_start();
	// 	QFile file;
	// 	QDataStream file_out;

	while (1)
	{
		packet * r_packet = (packet *)malloc(MAX_BUF);
		int len = sizeof(*addr);

		if ((recvfrom(recv_socket, (char *)r_packet, MAX_BUF, 0, (struct sockaddr *)addr, &len)) > 0)
		{
			// 校验和
			if (check_packet(r_packet))
			{
				switch (r_packet->OPT)
				{
					// V1.0
					case 1:
						// 建立连接 并且建立文件
						if (1 == r_packet->SYN)
						{
							if (0 == ACK)
							{
								ACK = r_packet->seq + 1;
							}
							else if (r_packet->seq == ACK)
							{
								std::string filename;
								for (int i = 0; i < r_packet->size; i ++)
									filename += (*(r_packet->data + i));
								emit startWrite(QString::fromStdString(filename));
								//file.setFileName(QString::fromStdString(filename));
								//file.open(QIODevice::WriteOnly);
								//file_out.setDevice(&file);
								ACK += r_packet->size;
							}
						}

						else if (1 == r_packet->PSH)
						{
							if (r_packet->seq == ACK)
							{
								m_eruptWrite->push_packet(r_packet);
								//file_out.writeRawData(r_packet->data, r_packet->size);
								//file.flush();
								ACK += r_packet->size;
							}
						}

						else if (1 == r_packet->FIN)
						{
							s_packet->seq = ACK + 1;
							for (int i = 0; i < 5; i ++)
							{
								sendto(recv_socket, (char *)s_packet, sizeof(packet) + s_packet->size + 1, 0, (struct sockaddr *)addr, sizeof(*addr));
							}

							while (!m_eruptWrite->is_finishsed()) {}
							m_eruptWrite->set_quit();
							del_socket(recv_socket);
							//net_close();

							emit finished();
							return;
						}

						break;

						// V2.0
					case 2:
						// 建立连接 并且建立文件
						if (1 == r_packet->SYN)
						{
							if (0 == ACK)
							{
								ACK = r_packet->seq + 1;
							}
							else if (r_packet->seq == ACK)
							{
								std::string filename;
								for (int i = 0; i < r_packet->size; i ++)
									filename += (*(r_packet->data + i));
								emit startWrite(QString::fromStdString(filename));
								ACK += r_packet->size;
							}
						}

						else if (1 == r_packet->PSH)
						{
							if (r_packet->seq == ACK)
							{
								m_eruptWrite->push_packet(r_packet);
								//file_out.writeRawData(r_packet->data, r_packet->size);
								//file.flush();
								ACK += r_packet->size;
							}
						}

						else if (1 == r_packet->FIN)
						{
							s_packet->seq = ACK + 1;
							for (int i = 0; i < 5; i ++)
							{
								sendto(recv_socket, (char *)s_packet, sizeof(packet) + s_packet->size + 1, 0, (struct sockaddr *)addr, sizeof(*addr));
								Sleep(5);
							}

							while (!m_eruptWrite->is_finishsed()) {}
							m_eruptWrite->set_quit();
							del_socket(recv_socket);
							//net_close();

							emit finished();
							return;
						}

						break;

						// V3.0
					case 3:
						break;
					default:
						break;
				}

				s_packet->seq = ACK;
				sendto(recv_socket, (char *)s_packet, sizeof(packet) + s_packet->size + 1, 0, (struct sockaddr *)addr, sizeof(*addr));
				Sleep(1);
			}
		}
	}

	del_socket(recv_socket);
	net_close();

	emit finished();
}

// 滑动窗口 接收端
void transmit::on_run2(QString recvIp, unsigned int recvPort)
{
}