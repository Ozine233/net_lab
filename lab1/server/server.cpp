#include "pch.h"
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <assert.h>
#include <map>
#include <vector>
#include <string>
#include <stdlib.h>
#include <sstream>
#include "protocol.h"

#pragma comment(lib,"ws2_32.lib")

#define MAX_BUF 200

using namespace std;

// 向线程传参结构体
typedef struct client_info
{
	string name;
	SOCKET socket;
	SOCKADDR_IN addr;
	int len = sizeof(addr);
}client_info;

// 在线的群组 (数组模拟链表)
const int N = 1000;
int h[N], ne[N], limit[N], idx;
client_info * e[N];

vector<string> groups;
int group_idx = -1;

// 客户端加入群组
bool join_group(client_info *client, int group_id)
{
	for(int i = h[group_id]; i != 0; i = ne[i])
		if(e[i] == client) return false;
	e[++idx] = client;
	ne[idx] = h[group_id];
	h[group_id] = idx;
	return true;
}

// 客户端退出群组
bool quit_group(client_info *client, int group_id)
{
	int p = 0;;
	for(int i = h[group_id]; i != 0; p = i, i = ne[i])
	{
		if(e[i] == client)
		{
			if(p == 0) h[group_id] = ne[i];
			else ne[p] = ne[i];
			return true;
		}
	}
	return false;
}

// 向群组内的客户端发消息
bool send_group(client_info * client, int group_id, const char * msg, int msg_len)
{
	for(int i = h[group_id]; i != 0; i = ne[i])
	{
		if(e[i] != client) 
			send(static_cast<client_info*>(client)->socket, msg, msg_len, 0);
	}
	return true;
}


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
	char buf[MAX_BUF];
	string message;
	client_info * client_now = static_cast<client_info*>(client);
	
	while (true)
	{
		// 获取格式化时间字符串
		SYSTEMTIME st;
		GetLocalTime(&st);
		char sDataTime[30];
		sprintf_s(sDataTime, "%4d-%2d-%2d %2d:%2d:%2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute,
		              st.wSecond);

		//重置缓存
		ZeroMemory(buf, MAX_BUF);
		if (recv(static_cast<client_info*>(client)->socket, buf, MAX_BUF, 0) != SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
			{
				Sleep(100);
				continue;
			}
			if (err == WSAETIMEDOUT || err == WSAENETDOWN)
			{
				closesocket(static_cast<client_info*>(client)->socket);
				return -1;
			}
		}

		string tmp;
		int group_id = -1;
		printf("%s,Recv From Client [%s:%d]:%s\n", sDataTime, inet_ntoa(static_cast<client_info*>(client)->addr.sin_addr), static_cast<client_info*>(client)->addr.sin_port,buf);
		
		// 将字节流转化为协议包
		chat_packet * packet = reinterpret_cast<chat_packet*>(buf);

		switch (packet->state_code)
		{
			// 发送消息
			case 'D':
				while(static_cast<istringstream>(packet->msg) >> tmp)
					if(group_id == -1)	group_id = atoi(tmp.c_str());
				tmp = static_cast<string>(sDataTime) + "-" + client_now->name + ":" + tmp;
				send_group(client_now, group_id, tmp.c_str(), tmp.size());
					
				break;
			
			// 注册名字
			case '@':
				client_now->name = packet->msg;
				break;
			
			// 列出在线的群
			case 'H':
				for(int i = 0; i <= group_idx; i ++)
					tmp += to_string(i) + groups[i] + "";
				send(client_now->socket, tmp.c_str(), tmp.size(), 0);
				break;
			
			// 加入某个群
			case 'L':
				group_id = atoi(packet->msg);
				if(limit[group_id])
				{
					send(client_now->socket, "加入失败，该群限制加入", strlen("加入失败，该群限制加入"), 0);
					break;
				}
				if(join_group(client_now, group_id))
					send(client_now->socket, "加入成功", strlen("加入成功"), 0);
				break;

			// 创建一个群
			case 'P':
				groups[++idx] = packet->msg;
				send(client_now->socket, "创建成功", strlen("创建成功"), 0);
				break;

			// 设置群为限制加入
			case 'T':
				group_id = atoi(packet->msg);
				limit[group_id] = 1;
				break;

			// 退出某个群聊
			case 'X':
				group_id = atoi(packet->msg);
				if(quit_group(client_now, group_id))
					send(client_now->socket, "退出成功", strlen("退出成功"), 0);
				break;

			// 退出程序
			case '\\':
				closesocket(client_now->socket);
				delete client_now;
				return 0;

			//丢弃非CCP协议数据包
			default:
				break;
		}
		
		//printf("%s,Recv From Client [%s:%d]:%s", sDataTime, inet_ntoa(static_cast<client_info*>(client)->addr.sin_addr), static_cast<client_info*>(client)->addr.sin_port,buf);
		//if (strcmp(buf, "quit") == 0)
		//{
		//	send(static_cast<client_info*>(client)->socket, "quit", strlen("quit"), 0);
		//	break;
		//}
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

		//阻塞socket
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