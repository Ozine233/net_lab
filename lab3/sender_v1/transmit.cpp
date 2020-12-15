#include "transmit.h"
#include <QDebug>
#include <QThread>
#include <QMessageBox>

transmit::transmit(QObject *parent /* = nullptr */)
{
}

void transmit::on_runV1(QString sendIp, unsigned int sendPort, QString recvIp, unsigned int recvPort, QString filepath)
{
	unsigned int MSS = 1000;
	bool is_connected = false;

	fileIO.init(filepath, MSS);
	fileinfo = QFileInfo(filepath);
	//std::ifstream fin(filepath.toStdString(), std::ios::binary);
	char str[MAX_BUF] = { 1 };

	//SOCKET send_Socket = socket(AF_INET, SOCK_DGRAM, 0);

	//// 设置接收端socket地址
	//SOCKADDR_IN addr;
	//addr.sin_family = AF_INET;
	//addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//addr.sin_port = htons(2333);

	//// 设置本地socket地址
	//SOCKADDR_IN my_addr;
	//my_addr.sin_family = AF_INET;
	//my_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//my_addr.sin_port = htons(2334);

	//bind(send_Socket, (SOCKADDR *)&my_addr, sizeof(SOCKADDR));

	//char test[] = "this is a test!";

	//auto packet = make_packet(0, 1, 0, 1, strlen(test), 0, test);

	////目地机的IP地址和端口号信息
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

		//初始化soclet
	SOCKADDR_IN *my_addr = get_addr(sendIp.toStdString(), sendPort);
	SOCKADDR_IN *aim_addr = get_addr(recvIp.toStdString(), recvPort);

	SOCKET send_socket = get_socket(my_addr);
	if (send_socket == NULL) QMessageBox::information(NULL, tr("Info"), tr("socket open failed! "));

	unsigned int seq = 0;

	//建立连接
	packet * s_packet = make_packet(1, 0, 0, 1, 0, 0, NULL);
	packet * r_packet = (packet *)malloc(MAX_BUF);

	int len = sizeof(*aim_addr);

	memset(r_packet, 0, MAX_BUF);
	while (!is_connected)
	{
		sendto(send_socket, (char *)s_packet, sizeof(packet) + s_packet->size + 1, 0, (struct sockaddr *)aim_addr, sizeof(*aim_addr));
		emit infoReady(tr("[SEND]--> SYN = 1 SEQ = ") + QString::number(seq), seq);
		//send_packet(send_socket, aim_addr, s_packet);
		if (recvfrom(send_socket, (char *)r_packet, MAX_BUF, 0, (sockaddr *)aim_addr, &len) > 0)
		{
			emit infoReady(tr("[RECV]<-- SYN = 1 ACK = ") + QString::number(r_packet->seq), seq);
			if (r_packet->seq == 1)
			{
				seq++;
				free(s_packet);
				s_packet = make_packet(1, 0, 0, 1, fileinfo.fileName().size(), seq, fileinfo.fileName().toStdString().c_str());
				is_connected = true;
			}
		}
		Sleep(1000);
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
		Sleep(1000);
	}
	int count = 0;

	while (!fileIO.is_end())
	{
		int read_len = 0;
		memset(str, 0, MAX_BUF);
		read_len += fileIO.read(str, MSS - read_len);

		//fin.read(str, MSS);

		//emit infoReady(QString("%1").arg(str), seq);
		s_packet = make_packet(0, 1, 0, 1, MSS, seq, str);
		while (count < 20)
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
					Sleep(50);
					break;
				}
			}
			Sleep(1000);
			count++;
		}

		if (count < 20) count = 0;
		else emit finished();
	}

	s_packet = make_packet(0, 0, 1, 1, 0, 1, NULL);

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
				break;
			}
		}
		Sleep(1000);
	}

	emit finished();
}

// V2
void transmit::on_runV2(QString sendIp, unsigned int sendPort, QString recvIp, unsigned int recvPort, QString filepath)
{
	fileIO.init(filepath, 1000);
	fileinfo = QFileInfo(filepath);
	char str[MAX_BUF] = { 0 };

	while (!fileIO.is_end())
	{
		memset(str, 0, MAX_BUF);
		fileIO.read(str);
		emit infoReady(QString("%1").arg(str), 0);
		Sleep(100);
	}
}

void transmit::on_runV3(QString sendIp, unsigned int sendPort, QString recvIp, unsigned int recvPort, QString filepath)
{
}