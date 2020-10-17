#include "pch.h"
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <assert.h>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

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

int main()
{
	// 初始化socket
	if(!start_socket()) cout << "socket alreay open!" << endl;
	else return -1;

	// 服务端socket 
	SOCKET serSocket = socket(AF_INET, SOCK_STREAM, 0);

	//设置非阻塞socket
	int iMode = 1;
	ioctlsocket(serSocket,FIONBIO, reinterpret_cast<u_long*>(&iMode));

	// 设置服务端socket地址
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(2333);

	// 绑定地址和服务端socket
	bind(serSocket, reinterpret_cast<SOCKADDR*>(&addr), sizeof(SOCKADDR));

	// 监听
	listen(serSocket, 5);

	//开始接受客户端请求
	cout << " Tcp server start..." << endl;

	SOCKET client_socket;
	SOCKADDR_IN client_addr;
	int client_addr_len = sizeof(client_addr);

	//循环等待
	while (true)
	{
		client_socket = accept(serSocket, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_len);
		if (INVALID_SOCKET == client_socket)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
			{
				Sleep(100);
				continue;
			}
			else
			{
				printf("accept failed!\n");
				end_socket(serSocket);
				return -1;
			}		
		}
		cout << "test" << endl;
		break;
	}

	char buf[60];
	while (true)
	{
		ZeroMemory(buf, 60);
		if (SOCKET_ERROR == recv(client_socket, buf, 60, 0))
		{
			int err = WSAGetLastError();
			if(err == WSAEWOULDBLOCK)
			{
				Sleep(100);
				continue;
			}
			if (err == WSAETIMEDOUT || err == WSAENETDOWN)
			{
				cout << "recv failed !" << endl;
				closesocket(client_socket);
				end_socket(serSocket);
				return -1;
			}
		}

		SYSTEMTIME st;
		GetLocalTime(&st);
		char sDataTime[30];
		sprintf_s(sDataTime, "%4d-%2d-%2d %2d:%2d:%2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		printf("%s,Recv From Client [%s:%d]:%s\n",sDataTime,inet_ntoa(client_addr.sin_addr),client_addr.sin_port,buf);
		if(strcmp(buf, "quit") == 0)
		{
			send(client_socket, "quit", strlen("quit"), 0);
			break;
		}
		
	}
 	
	getchar();
	
 	end_socket(serSocket);
	
	return 0;
}