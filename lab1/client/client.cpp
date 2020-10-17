#include "pch.h"
#pragma comment(lib,"ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>

using namespace std;

int main()
{
	WSADATA wsaData;
	const auto versionRequired = MAKEWORD(1, 1);
	const auto err = WSAStartup(versionRequired, &wsaData);

	if( !err)
	{
		cout << "socket already open!" << endl;
	}
	else
	{
		cout << "socket open falied!" << endl;
		return 0;
	}

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN clientsock_in;
	clientsock_in.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	clientsock_in.sin_family = AF_INET;
	clientsock_in.sin_port = htons(6000);

	connect(clientSocket, (SOCKADDR*)&clientsock_in, sizeof(SOCKADDR));

	char receiveBuf[100];

	recv(clientSocket, receiveBuf, 101, 0);
	
	cout << receiveBuf << endl;

	send(clientSocket, "hello, this is client!", strlen("hello, this is client!") + 1, 0);

	closesocket(clientSocket);

	WSACleanup();

	getchar();
	
	return 0;
}
