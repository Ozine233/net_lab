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

	SOCKET serSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(6000);

	bind(serSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR));
	listen(serSocket, 5);

	SOCKADDR_IN clientsocket;
	int len = sizeof(SOCKADDR);
	while (1)
	{
		SOCKET serConn = accept(serSocket, (SOCKADDR*)&clientsocket, &len);

		char sendBuf[100];
		sprintf_s(sendBuf, "welcome %s to being", inet_ntoa(clientsocket.sin_addr));
		send(serConn, sendBuf, strlen(sendBuf) + 1, 0);

		char receiveBuf[100];
		recv(serConn, receiveBuf, strlen(receiveBuf) + 1, 0);
		cout << receiveBuf << endl;
		closesocket(serConn);
		WSACleanup();
	}

	
	return 0;
}