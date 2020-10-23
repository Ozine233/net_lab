#include "pch.h"
#include <iostream>
#include <WinSock2.h>
#include <assert.h>
#include <string>
#include "protocol.h"

#define MAX_BUF 200

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

	cout << "start connect.." << endl;

	while(true)
	{
		string send_msg;
		cin >> send_msg;

		chat_packet * packet;
		
		if(send_msg == "list")
		{
			packet = make_packet('H', send_msg.size(), send_msg.c_str());
			send(clientSocket, reinterpret_cast<char*>(packet), sizeof(packet), 0);
		}
		if(send_msg == "create")
		{
			string group_id;
			cin >> group_id;
			packet = make_packet('P', group_id.size(), group_id.c_str());
		}
		if(send_msg == "quit") break;


		// 接受消息
		char receiveBuf[MAX_BUF];
		ZeroMemory(receiveBuf, MAX_BUF);
		recv(clientSocket, receiveBuf, MAX_BUF, 0);
		cout << receiveBuf << endl;
		
	}


	
	
	closesocket(clientSocket);
	WSACleanup();

	getchar();
	
	return 0;
}
