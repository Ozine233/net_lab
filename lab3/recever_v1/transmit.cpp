#include "transmit.h"
#include <QDebug>
#include <QThread>
#include <QMessageBox>
#include <vector>
#include <algorithm>
#include <queue>
#include <utility>

transmit::transmit(QObject *parent /* = nullptr */)
{
	m_eruptWrite = new eruptWrite();
	m_eruptWrite->moveToThread(&m_eruptWrite_thread);
	m_eruptWrite_thread.start();

	// �����źŲ�
	connect(this, &transmit::startWrite, m_eruptWrite, &eruptWrite::on_write);
}

transmit::~transmit()
{
	m_eruptWrite_thread.quit();
	m_eruptWrite_thread.wait();
}

// ͣ�Ȼ���  ���ն�
void transmit::on_run1(QString recvIp, unsigned int recvPort)
{
	// 	SOCKET recv_Socket = socket(AF_INET, SOCK_DGRAM, 0);
	//
	// 	// ���ý��ն�socket��ַ
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
	// 		//s����ʶһ���������׽ӿڵ������֡�
	// 		//buf���������ݻ�������
	// 		//len�����������ȡ�
	// 		//flags�����ò�����ʽ��
	// 		//from������ѡ��ָ�룬ָ��װ��Դ��ַ�Ļ�������
	// 		//fromlen������ѡ��ָ�룬ָ��from����������ֵ��
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

	SOCKADDR_IN *addr = get_addr(recvIp.toStdString(), recvPort);
	SOCKET recv_socket = get_socket(addr);

	packet * s_packet = make_packet(0, 0, 0, 0, 0, NULL);
	unsigned int ACK = 0;

	m_eruptWrite->set_start();
	// 	QFile file;
	// 	QDataStream file_out;

	packet * r_packet = (packet *)malloc(MAX_BUF);
	int len = sizeof(*addr);
	while (1)
	{
		if ((recvfrom(recv_socket, (char *)r_packet, MAX_BUF, 0, (struct sockaddr *)addr, &len)) > 0)
		{
			// У���
			if (check_packet(r_packet))
			{
				// �������� ���ҽ����ļ�
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

				else if (1 == r_packet->PSH)
				{
					if (r_packet->seq == ACK)
					{
						m_eruptWrite->push_packet(r_packet);
						//file_out.writeRawData(r_packet->data, r_packet->size);
						//file.flush();
						ACK += r_packet->size;
						r_packet = (packet *)malloc(MAX_BUF);
					}
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

#define PPI std::pair<packet*, unsigned int>

// �������� ���ն�
void transmit::on_run2(QString recvIp, unsigned int recvPort)
{
	// ����socket
	SOCKADDR_IN *addr = get_addr(recvIp.toStdString(), recvPort);
	SOCKET recv_socket = get_socket(addr);

	// ��ʼ��io
	m_eruptWrite->set_start();

	packet * s_packet = make_packet(0, 0, 0, 0, 0, NULL);

	// Ԥ����MSS�Ĵ�С
	unsigned int MSS = 2000;

	// ��ʼ�����ջ��� (��С��)
	struct cmp
	{
		bool operator()(const PPI&a, const PPI&b)
		{
			return a.second > b.second;
		}
	};
	std::priority_queue<PPI, std::vector<PPI>, cmp> recvBuffer;

	unsigned int ACK = 0;

	// �ٶ��Ļ�������С
	const int windSize = 20;

	// Ϊ�������ӷ��仺��
	packet * r_packet = (packet *)malloc(MAX_BUF);
	int len = sizeof(*addr);
	while (1)
	{
		Sleep(1);
		if ((recvfrom(recv_socket, (char *)r_packet, MAX_BUF, 0, (struct sockaddr *)addr, &len)) > 0)
		{
			// У���
			if (check_packet(r_packet))
			{
				// �������� ���ҽ����ļ�
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

				else if (1 == r_packet->PSH)
				{
					if (r_packet->seq >= ACK)
					{
						recvBuffer.push(std::make_pair(r_packet, r_packet->seq));
						r_packet = (packet *)malloc(MAX_BUF);
						if (recvBuffer.size() < windSize) continue;
					}

					while (!recvBuffer.empty() && recvBuffer.top().second <= ACK)
					{
						packet * tmp_packet = recvBuffer.top().first;
						recvBuffer.pop();
						if (tmp_packet->seq == ACK)
						{
							ACK += tmp_packet->size;
							m_eruptWrite->push_packet(tmp_packet);
						}
						else
						{
							free(tmp_packet);
						}
					}
				}

				s_packet->seq = ACK;
				sendto(recv_socket, (char *)s_packet, sizeof(packet) + s_packet->size + 1, 0, (struct sockaddr *)addr, sizeof(*addr));
			}
		}
	}
}