#include "pch.h"
#include <iostream>
#include <WinSock2.h>
#include <string>
#include <errno.h>
#include "protocol.h"

#define MAX_BUF 200
#define SEND_BUF 3 + packet->msg_len

#pragma comment(lib,"ws2_32.lib")

using namespace std;

// 检测socket对应版本开启检查
bool start_socket()
{
	WSADATA wsaData;
	const auto versionRequired = MAKEWORD(2, 2);
	return WSAStartup(versionRequired, &wsaData);
}

// 循环接受消息线程
DWORD WINAPI recv_msg(LPVOID clientsocket)
{
	while (true)
	{
		char receiveBuf[MAX_BUF];
		ZeroMemory(receiveBuf, MAX_BUF);
		recv(reinterpret_cast<SOCKET>(clientsocket), receiveBuf, MAX_BUF, 0);
		if(receiveBuf) cout << "<--\n" << receiveBuf << "\n-->" << endl << ">>";
		Sleep(10);
	}
	
}

int main()
{
	// 初始化socket
	if (!start_socket()) cout << "socket alreay open!" << endl;
	else return -1;

	// 客户端socke
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	// 设置socket地址
	SOCKADDR_IN clientsock_in;
	clientsock_in.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	clientsock_in.sin_family = AF_INET;
	clientsock_in.sin_port = htons(2333);

	//SOCKADDR_IN server_in;
	//server_in.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//server_in.sin_family = AF_INET;
	//server_in.sin_port = htons(2333);

	// 连接服务器
	while(true)
	{
		int reval = connect(clientSocket, (SOCKADDR*)&clientsock_in, sizeof(SOCKADDR));

		if(reval == 0) break;
		cout << reval << " " << errno << endl;
		Sleep(1000);
	}
	

	// 创建接收消息线程
	DWORD ThreadId;
	HANDLE recv_thread = CreateThread(nullptr, 0, recv_msg, reinterpret_cast<LPVOID>(clientSocket), 0, &ThreadId);
	CloseHandle(recv_thread);

	//退出标志
	bool is_exit = false;
	cout << "start connect.." << endl;
	cout << ">>";
	
	// 主循环
	while(!is_exit)
	{
		int code = 1;
		string send_msg;
		cin >> send_msg;

		chat_packet * packet = nullptr;

		// 注册名字
		if(send_msg == "register")
		{
			cin >> send_msg;
		}

		// 发送消息
		else if(send_msg == "send")
		{
			code <<= 1;
			string tmp;
			cin >> send_msg >> tmp;
			send_msg += " " + tmp;
		}
		
		// 列出在线的群
		else if(send_msg == "list")
		{
			code <<= 2;
		}

		// 加入一个群
		else if(send_msg == "join")
		{
			code <<= 3;
			cin >> send_msg;
		}

		// 创建一个群
		else if(send_msg == "create")
		{
			code <<= 4;
			cin >> send_msg;
		}

		// 设置群为限制加入
		else if(send_msg == "limit")
		{
			code <<= 5;
			cin >> send_msg;
		}

		// 退出某个群聊
		else if(send_msg == "quit")
		{
			code <<= 6;
			cin >> send_msg;
		}

		// 退出
		else if(send_msg == "exit")
		{
			code <<= 7;
			is_exit = true;
		}

		else
		{
			cout << "<- " << "指令不合法" << " ->" << endl;
			cout << ">>";
			continue;
		}
		
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

	// 关闭socket
	closesocket(clientSocket);
	WSACleanup();
	
	return 0;
}
