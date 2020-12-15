#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <string>
#include "protocol.h"

#pragma comment(lib,"ws2_32.lib")

using namespace std;

int main()
{
	// 初始化socket
	// 开启socket版本检查
	WSADATA wsaData;
	const auto versionRequired = MAKEWORD(2, 2);
	if (!WSAStartup(versionRequired, &wsaData)) cout << "socket alreay open!" << endl;
	else return -1;

	// 发送端socket
	SOCKET send_Socket = socket(AF_INET, SOCK_DGRAM, 0);

	// 设置接收端socket地址
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(2333);

	SOCKADDR_IN my_addr;
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	my_addr.sin_port = htons(2334);

	//绑定地址和接收端socket
	if (bind(send_Socket, (SOCKADDR*)&my_addr, sizeof(SOCKADDR)) < 0)
	{
		cout << "套接字描述符与套接字地址绑定失败!" << endl;
		// 关闭socket
		closesocket(send_Socket);
		WSACleanup();
		return 0;
	}

	// sendto
	char test[] = "this is a test!";

	auto packet = make_packet(0, 1, 0, strlen(test), 0, test);

	//目地机的IP地址和端口号信息
	sendto(send_Socket, (char *)packet, sizeof(struct packet) + packet->size + 1, 0, (struct sockaddr *)&addr, sizeof(addr));

	while (1)
	{
		char recvstr[1000];
		int len = sizeof(addr);
		int recvnum = recvfrom(send_Socket, recvstr, 1000, 0, (struct sockaddr *)&addr, &len);
		if (recvnum > 0)
		{
			recvstr[recvnum] = '\0';
			printf("服务器端发来的数据为:\n%s\n", recvstr);
		}
	}

	closesocket(send_Socket);
	WSACleanup();
	return 0;
}