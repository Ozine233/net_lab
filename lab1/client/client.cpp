#include "pch.h"
#include <iostream>
#include <WinSock2.h>
#include <assert.h>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

bool start_socket()
{
	WSADATA wsaData;
	const auto versionRequired = MAKEWORD(2, 2);
	return WSAStartup(versionRequired, &wsaData);
}

int main()
{
	assert(!start_socket());
	
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN clientsock_in;
	clientsock_in.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	clientsock_in.sin_family = AF_INET;
	clientsock_in.sin_port = htons(2333);

	connect(clientSocket, (SOCKADDR*)&clientsock_in, sizeof(SOCKADDR));

	char send_msg[60];
	ZeroMemory(send_msg, 60);
	cout << "start connect.." << endl;
	while(true)
	{
		cin >> send_msg;
		send(clientSocket, send_msg, 61, 0);
		ZeroMemory(send_msg, 60);
		if(strcmp(send_msg, "quit") == 0) break;
	}


	char receiveBuf[100];
	ZeroMemory(receiveBuf, 100);

	recv(clientSocket, receiveBuf, 101, 0);

	cout << receiveBuf << endl;
	
	closesocket(clientSocket);
	WSACleanup();

	getchar();
	
	return 0;
}
