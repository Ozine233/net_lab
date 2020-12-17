#include "transmit.h"
#include <QDebug>
#include <QThread>
#include <QMessageBox>
#include <time.h>
#include <queue>

#define MSS 2000

transmit::transmit(QObject *parent /* = nullptr */)
{
}

void transmit::on_runV1(QString sendIp, unsigned int sendPort, QString recvIp, unsigned int recvPort, QString filepath)
{
	// ��¼����ʱ��
	double time_start = (double)clock();

	fileIO.init(filepath, MSS);
	char str[MAX_BUF] = { 0 };

	//SOCKET send_Socket = socket(AF_INET, SOCK_DGRAM, 0);
	//// ���ý��ն�socket��ַ
	//SOCKADDR_IN addr;
	//addr.sin_family = AF_INET;
	//addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//addr.sin_port = htons(2333);
	//// ���ñ���socket��ַ
	//SOCKADDR_IN my_addr;
	//my_addr.sin_family = AF_INET;
	//my_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//my_addr.sin_port = htons(2334);
	//bind(send_Socket, (SOCKADDR *)&my_addr, sizeof(SOCKADDR));
	//char test[] = "this is a test!";
	//auto packet = make_packet(0, 1, 0, 1, strlen(test), 0, test);
	////Ŀ�ػ���IP��ַ�Ͷ˿ں���Ϣ
	//sendto(send_Socket, (char *)packet, 9 + packet->size, 0, (struct sockaddr *)&addr, sizeof(addr));
	//while (1)
	//{
	//	char recvstr[80];
	//	int len = sizeof(addr);
	//	int recvnum = recvfrom(send_Socket, recvstr, 80, 0, (struct sockaddr *)&addr, &len);
	//	if (recvnum > 0)
	//	{
	//		recvstr[recvnum] = '\0';
	//		qDebug("!!!!!\n%s", recvstr);
	//	}
	//}

	//��ʼ��soclet
	SOCKADDR_IN *my_addr = get_addr(sendIp.toStdString(), sendPort);
	SOCKADDR_IN *aim_addr = get_addr(recvIp.toStdString(), recvPort);

	SOCKET send_socket = get_socket(my_addr);

	// ��ʼ��ά������
	unsigned int seq = 0;
	bool is_connected = false;

	// �������� ���� ��Զ���½��ļ�
	packet * s_packet = make_packet(1, 0, 0, 0, 0, NULL);
	packet * r_packet = (packet *)malloc(MAX_BUF);

	int len = sizeof(*aim_addr);

	memset(r_packet, 0, MAX_BUF);
	while (!is_connected)
	{
		sendto(send_socket, (char *)s_packet, sizeof(packet) + s_packet->size + 1, 0, (struct sockaddr *)aim_addr, sizeof(*aim_addr));
		emit infoReady(tr("[SEND]--> SYN = 1 SEQ = ") + QString::number(seq), seq);
		if (recvfrom(send_socket, (char *)r_packet, MAX_BUF, 0, (sockaddr *)aim_addr, &len) > 0)
		{
			emit infoReady(tr("[RECV]<-- SYN = 1 ACK = ") + QString::number(r_packet->seq), seq);
			if (r_packet->seq == 1)
			{
				seq++;
				free(s_packet);
				s_packet = make_packet(1, 0, 0, fileIO.get_filename().size(), seq, fileIO.get_filename().toStdString().c_str());
				is_connected = true;
			}
		}
		Sleep(50);
	}

	memset(r_packet, 0, MAX_BUF);
	bool setup_filename = false;
	while (!setup_filename)
	{
		sendto(send_socket, (char *)s_packet, sizeof(packet) + s_packet->size + 1, 0, (struct sockaddr *)aim_addr, sizeof(*aim_addr));
		emit infoReady(tr("[SEND]--> SYN = 1 SEQ = ") + QString::number(seq), seq);
		if (recvfrom(send_socket, (char *)r_packet, MAX_BUF, 0, (sockaddr *)aim_addr, &len) > 0)
		{
			emit infoReady(tr("[RECV]<-- SYB = 1 ACK = ") + QString::number(r_packet->seq), seq);
			if (r_packet->seq == seq + s_packet->size)
			{
				seq = r_packet->seq;
				free(s_packet);
				setup_filename = true;
			}
		}
		Sleep(50);
	}

	int count = 0;
	while (!fileIO.is_end())
	{
		memset(str, 0, MAX_BUF);
		unsigned int read_len = fileIO.read(str);

		//fin.read(str, MSS);
		//emit infoReady(QString("%1").arg(str), seq);

		s_packet = make_packet(0, 1, 0, read_len, seq, str);
		while (count < 10)
		{
			memset(r_packet, 0, MAX_BUF);
			sendto(send_socket, (char *)s_packet, sizeof(packet) + s_packet->size + 1, 0, (struct sockaddr *)aim_addr, sizeof(*aim_addr));
			emit infoReady(tr("[SEND]--> PSH = 1 seq = ") + QString::number(seq), seq);
			if (recvfrom(send_socket, (char *)r_packet, MAX_BUF, 0, (sockaddr *)aim_addr, &len) > 0)
			{
				emit infoReady(tr("[RECV]<-- ACK = ") + QString::number(r_packet->seq), seq);
				if (r_packet->seq == seq + s_packet->size)
				{
					seq = r_packet->seq;
					free(s_packet);
					//Sleep(50);
					break;
				}
			}
			Sleep(10);
			count++;
		}

		if (count < 50) count = 0;
		else emit finished(0.0);
	}

	s_packet = make_packet(0, 0, 1, 0, 1, NULL);

	bool is_reply = false;
	while (!is_reply)
	{
		memset(r_packet, 0, MAX_BUF);
		sendto(send_socket, (char *)s_packet, sizeof(packet) + s_packet->size + 1, 0, (struct sockaddr *)aim_addr, sizeof(*aim_addr));
		emit infoReady(tr("[SEND] FIN = 1 seq = ") + QString::number(seq), seq);
		if (recvfrom(send_socket, (char *)r_packet, MAX_BUF, 0, (sockaddr *)aim_addr, &len) > 0)
		{
			emit infoReady(tr("[RECV] ACK = ") + QString::number(r_packet->seq), seq);
			if (r_packet->seq == seq + 1)
			{
				seq = r_packet->seq;
				free(s_packet);
				is_reply = true;
			}
		}
		Sleep(50);
	}

	double time_end = (double)clock();
	double time_spend = (time_end - time_start) / 1000.0;
	del_socket(send_socket);
	emit finished(time_spend);
}

// V2
void transmit::on_runV2(QString sendIp, unsigned int sendPort, QString recvIp, unsigned int recvPort, QString filepath)
{
	// ��¼��ʼʱ��
	double time_start = (double)clock();

	// ��ʼ�����ļ��Ķ�ȡio
	fileIO.init(filepath, MSS);
	char str[MAX_BUF] = { 0 };

	//��ʼ��soclet
	SOCKADDR_IN *my_addr = get_addr(sendIp.toStdString(), sendPort);
	SOCKADDR_IN *aim_addr = get_addr(recvIp.toStdString(), recvPort);
	SOCKET send_socket = get_socket(my_addr);

	// �������� ���� ��Զ���½��ļ�
	unsigned int seq = 0;
	packet * s_packet = make_packet(1, 0, 0, 0, 0, NULL);
	packet * r_packet = (packet *)malloc(MAX_BUF);

	int len = sizeof(*aim_addr);

	bool is_connected = false;
	memset(r_packet, 0, MAX_BUF);
	while (!is_connected)
	{
		sendto(send_socket, (char *)s_packet, sizeof(packet) + s_packet->size + 1, 0, (struct sockaddr *)aim_addr, sizeof(*aim_addr));
		emit infoReady(tr("[SEND]--> SYN = 1 SEQ = ") + QString::number(seq), seq);
		if (recvfrom(send_socket, (char *)r_packet, MAX_BUF, 0, (sockaddr *)aim_addr, &len) > 0)
		{
			emit infoReady(tr("[RECV]<-- SYN = 1 ACK = ") + QString::number(r_packet->seq), seq);
			if (r_packet->seq == 1)
			{
				seq++;
				free(s_packet);
				s_packet = make_packet(1, 0, 0, fileIO.get_filename().size(), seq, fileIO.get_filename().toStdString().c_str());
				is_connected = true;
			}
		}
		Sleep(50);
	}

	bool setup_filename = false;
	memset(r_packet, 0, MAX_BUF);
	while (!setup_filename)
	{
		sendto(send_socket, (char *)s_packet, sizeof(packet) + s_packet->size + 1, 0, (struct sockaddr *)aim_addr, sizeof(*aim_addr));
		emit infoReady(tr("[SEND]--> SYN = 1 SEQ = ") + QString::number(seq), seq);
		if (recvfrom(send_socket, (char *)r_packet, MAX_BUF, 0, (sockaddr *)aim_addr, &len) > 0)
		{
			emit infoReady(tr("[RECV]<-- SYB = 1 ACK = ") + QString::number(r_packet->seq), seq);
			if (r_packet->seq == seq + s_packet->size)
			{
				seq = r_packet->seq;
				free(s_packet);
				setup_filename = true;
			}
		}
		Sleep(50);
	}

	// ���ڴ�С
	unsigned int windSize = 40000;
	unsigned int LastAck = seq;
	unsigned int LastSend = seq;

	// ��������ݰ�
	std::queue<packet *> sendBuffer;

	// ������ѭ��
	while (!fileIO.is_end())
	{
		while (!fileIO.is_end() && LastSend - LastAck < windSize)
		{
			memset(str, 0, MAX_BUF);
			unsigned int read_len = fileIO.read(str);
			s_packet = make_packet(0, 1, 0, read_len, LastSend, str);
			sendBuffer.push(s_packet);
			// �������ݰ�
			sendto(send_socket, (char *)s_packet, sizeof(packet) + s_packet->size + 1, 0, (struct sockaddr *)aim_addr, sizeof(*aim_addr));
			emit infoReady(tr("[SEND]--> PSH = 1 seq = ") + QString::number(LastSend), LastAck);
			LastSend += s_packet->size;
		}

		// ����ACK
		int time_count = 5;
		while (time_count --)
		{
			if (recvfrom(send_socket, (char *)r_packet, MAX_BUF, 0, (sockaddr *)aim_addr, &len) > 0)
			{
				emit infoReady(tr("[RECV]<-- ACK = ") + QString::number(r_packet->seq), LastAck);
				if (r_packet->seq > LastAck && r_packet->seq <= (LastSend + sendBuffer.back()->size))
				{
					//unsigned int n = (r_packet->seq - LastAck) / MSS;
					//while (n --)
					//{
					//	packet *free_packet = sendBuffer.front();
					//	sendBuffer.pop();
					//	free(free_packet);
					//}
					while (LastAck != r_packet->seq)
					{
						packet *free_packet = sendBuffer.front();
						sendBuffer.pop();
						LastAck += free_packet->size;
						free(free_packet);
					}
					break;
				}
			}
			Sleep(10);
		}
		if (time_count > 0) continue;

		// �ش�
		int tmp = sendBuffer.size();
		int base_ACK = LastAck;
		while (tmp --)
		{
			packet * tmp_packet = sendBuffer.front();
			sendto(send_socket, (char *)tmp_packet, sizeof(packet) + tmp_packet->size + 1, 0, (struct sockaddr *)aim_addr, sizeof(*aim_addr));
			emit infoReady(tr("[SEND]--> PSH = 1 seq = ") + QString::number(base_ACK), LastAck);
			base_ACK += tmp_packet->size;
			sendBuffer.pop();
			sendBuffer.push(tmp_packet);
		}
	}

	// ȷ���Ƿ���
	while (LastAck > LastSend)
	{
		// �ش�
		int tmp = sendBuffer.size();
		int base_ACK = LastAck;
		while (tmp --)
		{
			packet * tmp_packet = sendBuffer.front();
			sendto(send_socket, (char *)tmp_packet, sizeof(packet) + tmp_packet->size + 1, 0, (struct sockaddr *)aim_addr, sizeof(*aim_addr));
			emit infoReady(tr("[SEND]--> PSH = 1 seq = ") + QString::number(base_ACK), LastAck);
			base_ACK += tmp_packet->size;
			sendBuffer.pop();
			sendBuffer.push(tmp_packet);
		}

		int count = 5;
		while (count --)
		{
			if (recvfrom(send_socket, (char *)r_packet, MAX_BUF, 0, (sockaddr *)aim_addr, &len) > 0)
			{
				emit infoReady(tr("[RECV]<-- ACK = ") + QString::number(r_packet->seq), LastAck);
				if (r_packet->seq > LastAck && r_packet->seq <= (LastSend + sendBuffer.back()->size))
				{
					while (LastAck != r_packet->seq)
					{
						packet *free_packet = sendBuffer.front();
						sendBuffer.pop();
						LastAck += free_packet->size;
						free(free_packet);
					}
					break;
				}
			}
			Sleep(10);
		}
		if (count > 0) continue;
	}

	// ȷ��finish
	s_packet = make_packet(0, 0, 1, 0, 1, NULL);

	bool is_reply = false;
	int count = 0;
	while (!is_reply)
	{
		sendto(send_socket, (char *)s_packet, sizeof(packet) + s_packet->size + 1, 0, (struct sockaddr *)aim_addr, sizeof(*aim_addr));
		emit infoReady(tr("[SEND] FIN = 1 seq = ") + QString::number(LastAck), LastAck);
		if (recvfrom(send_socket, (char *)r_packet, MAX_BUF, 0, (sockaddr *)aim_addr, &len) > 0)
		{
			emit infoReady(tr("[RECV] ACK = ") + QString::number(r_packet->seq), LastAck);
			if (r_packet->seq == LastAck + 1)
			{
				free(s_packet);
				is_reply = true;
			}
		}
		Sleep(10);
		if (++count > 10) break;
	}

	// ��¼����ʱ�䲢�����ʱ
	double time_end = (double)clock();
	double time_spend = (double)(time_end - time_start) / 1000.0;
	del_socket(send_socket);
	// ���������ź�
	emit finished(time_spend);
}

void transmit::on_runV3(QString sendIp, unsigned int sendPort, QString recvIp, unsigned int recvPort, QString filepath)
{
}