#include "pch.h"
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <assert.h>
#include "protocol.h"

#pragma comment(lib,"ws2_32.lib")

using namespace std;

// 向线程传参结构体
typedef struct client_info
{
	SOCKET socket;
	SOCKADDR_IN addr;
	int len = sizeof(addr);
}client_info;

// 检测socket对应版本开启检查
bool start_socket()
{
	WSADATA wsaData;
	const auto versionRequired = MAKEWORD(2, 2);
	return WSAStartup(versionRequired, &wsaData);
}

// 释放socket
void end_socket(SOCKET& socket_to_close)
{
	closesocket(socket_to_close);
	WSACleanup();
}

// 处理客户端TCP连接
DWORD WINAPI deal_client(LPVOID client)
{
	char buf[60];
	while (true)
	{
		ZeroMemory(buf, 60);
		if (recv(static_cast<client_info*>(client)->socket, buf, 60, 0) != SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
			{
				Sleep(100);
				continue;
			}
			if (err == WSAETIMEDOUT || err == WSAENETDOWN)
			{
				cout << "recv failed !" << endl;
				closesocket(static_cast<client_info*>(client)->socket);
				return -1;
			}
		}

		SYSTEMTIME st;
		GetLocalTime(&st);
		char sDataTime[30];
		sprintf_s(sDataTime, "%4d-%2d-%2d %2d:%2d:%2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		printf("%s,Recv From Client [%s:%d]:%s\n", sDataTime, inet_ntoa(static_cast<client_info*>(client)->addr.sin_addr), static_cast<client_info*>(client)->addr.sin_port,
		       buf);
		if (strcmp(buf, "quit") == 0)
		{
			send(static_cast<client_info*>(client)->socket, "quit", strlen("quit"), 0);
			break;
		}
	}
	
	closesocket(static_cast<client_info*>(client)->socket);
	delete static_cast<client_info*>(client);
	
	return 0;
}

int main()
{
	// 初始化socket
	if(!start_socket()) cout << "socket alreay open!" << endl;
	else return -1;

	// 服务端socket 
	SOCKET serSocket = socket(AF_INET, SOCK_STREAM, 0);

	//设置非阻塞socket
	//int iMode = 1;
	//ioctlsocket(serSocket,FIONBIO, reinterpret_cast<u_long*>(&iMode));

	// 设置服务端socket地址
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(2333);

	// 绑定地址和服务端socket
	bind(serSocket, reinterpret_cast<SOCKADDR*>(&addr), sizeof(SOCKADDR));

	// 监听
	listen(serSocket, 5);

	// 开始接受客户端请求
	cout << " Tcp server start..." << endl;

	// 循环等待
	while (true)
	{
		client_info* client = new client_info;
		
		client->socket = accept(serSocket, reinterpret_cast<sockaddr*>(&client->addr), &client->len);
		if (INVALID_SOCKET == client->socket)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
			{
				Sleep(100);
				continue;
			}
			else
			{
				cout << "accept failed!" << endl;
				end_socket(serSocket);
				return -1;
			}		
		}

		// 未报错则处理客户端连接
		DWORD ThreadId;
		HANDLE deal_thread = CreateThread(nullptr, 0, deal_client, reinterpret_cast<LPVOID>(client),0, &ThreadId);
		CloseHandle(deal_thread);
	}
	
	getchar();
	
 	end_socket(serSocket);
	
	return 0;
}