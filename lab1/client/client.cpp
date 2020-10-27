#include "pch.h"
#include <iostream>
#include <WinSock2.h>
#include <assert.h>
#include <string>
#include "protocol.h"

#define MAX_BUF 200
#define SEND_BUF 3 + packet->msg_len

#pragma comment(lib,"ws2_32.lib")

using namespace std;

bool start_socket()
{
	WSADATA wsaData;
	const auto versionRequired = MAKEWORD(2, 2);
	return WSAStartup(versionRequired, &wsaData);
}

// 接受消息
DWORD WINAPI recv_msg(LPVOID clientsocket)
{
	while (true)
	{
		char receiveBuf[MAX_BUF];
		ZeroMemory(receiveBuf, MAX_BUF);
		recv(reinterpret_cast<SOCKET>(clientsocket), receiveBuf, MAX_BUF, 0);
		if(receiveBuf) cout << receiveBuf << endl;
	}
	return 0;
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

	DWORD ThreadId;
	HANDLE recv_thread = CreateThread(nullptr, 0, recv_msg, reinterpret_cast<LPVOID>(clientSocket), 0, &ThreadId);
	CloseHandle(recv_thread);

	cout << "start connect.." << endl;

	while(true)
	{
		int code = 1;
		string send_msg;
		cout << ">>";
		cin >> send_msg;

		chat_packet * packet = nullptr;

		// 注册名字
		if(send_msg == "register")
		{
			cin >> send_msg;
		}

		// 发送消息
		if(send_msg == "send")
		{
			code <<= 1;
			string tmp;
			cin >> send_msg >> tmp;
			send_msg += " " + tmp;
		}
		
		// 列出在线的群
		if(send_msg == "list")
		{
			code <<= 2;
		}

		// 加入一个群
		if(send_msg == "join")
		{
			code <<= 3;
			cin >> send_msg;
		}

		// 创建一个群
		if(send_msg == "create")
		{
			code <<= 4;
			cin >> send_msg;
		}

		// 退出
		if(send_msg == "quit") break;
		
		// 封包
		packet = make_packet(static_cast<byte>(code), send_msg.size(), send_msg.c_str());

		// 发送消息
		send(clientSocket, reinterpret_cast<char*>(packet), SEND_BUF, 0);
		
		// 释放packet内存
		if(packet != nullptr)
			free(packet);

		// 等待
		Sleep(5);
	}

	closesocket(clientSocket);
	WSACleanup();

	getchar();
	
	return 0;
}
