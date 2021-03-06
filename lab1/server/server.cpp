#include "pch.h"
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <assert.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include <sstream>
#include "protocol.h"

#pragma comment(lib,"ws2_32.lib")

#define MAX_BUF 300
#define MISSING_NAME "匿名"

using namespace std;


// low bit 操作
int lowbit(int x)
{
	return x & (-x);
}

// 向线程传参结构体
typedef struct client_info
{
	string name;
	SOCKET socket;
	SOCKADDR_IN addr;
	int len = sizeof(addr);
} client_info;

// 在线的群组 (数组模拟链表)
const int N = 1000;
int h[N], ne[N], limit[N], idx;
client_info* e[N];

vector<string> groups;
int group_idx = -1;

// 客户端是否在群组
bool is_inGroup(client_info* client, int group_id)
{
	for(int i = h[group_id]; i != 0; i = ne[i])
		if(e[i] == client) return true;
	return false;
}

// 客户端加入群组
bool join_group(client_info* client, int group_id)
{
	for (int i = h[group_id]; i != 0; i = ne[i])
		if (e[i] == client) return false;
	e[++idx] = client;
	ne[idx] = h[group_id];
	h[group_id] = idx;
	return true;
}

// 客户端退出群组
bool quit_group(client_info* client, int group_id)
{
	int p = 0;;
	for (int i = h[group_id]; i != 0; p = i, i = ne[i])
	{
		if (e[i] == client)
		{
			if (p == 0) h[group_id] = ne[i];
			else ne[p] = ne[i];
			return true;
		}
	}
	return false;
}

// 向群组内的客户端发消息
bool send_group(client_info* client, int group_id, const char* msg, int msg_len)
{
	for (int i = h[group_id]; i != 0; i = ne[i])
	{
		if (e[i] != client)
			send(e[i]->socket, msg, msg_len, 0);
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
	client_info* client_now = static_cast<client_info*>(client);

	// 设置匿名name
	client_now->name = MISSING_NAME;

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
		if (recv(client_now->socket, buf, MAX_BUF, 0) != SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
			{
				Sleep(100);
				continue;
			}
			if (err == WSAETIMEDOUT || err == WSAENETDOWN)
			{
				closesocket(client_now->socket);
				return -1;
			}
		}

		string tmp = "";
		stringstream ss_tmp;
		int group_id = -1;

		// 将字节流转化为协议包
		chat_packet* packet = reinterpret_cast<chat_packet*>(buf);
		
		// 打印日志
		printf("[INFO]%s,Recv From Client [%s:%d]:%x %s\n", sDataTime,
		       inet_ntoa(client_now->addr.sin_addr),
		       client_now->addr.sin_port, packet->state_code, packet->msg);

		switch (lowbit(static_cast<int>(packet->state_code)))
		{
			// 注册名字
		case 1:
			client_now->name = packet->msg;
			send(client_now->socket, "注册成功", strlen("注册成功"), 0);
			break;

			// 发送消息
		case 2:
			// 切分字符串
			tmp = packet->msg;
			ss_tmp.str(tmp);
			while(ss_tmp >> tmp) if(group_id == -1) group_id = atoi(tmp.c_str());

			// 检测客户端是否在该群中
			if(is_inGroup(client_now, group_id))
				tmp = "[" + static_cast<string>(sDataTime) + "]" + "[" + client_now->name + "]:" + tmp;
			else
			{
				send(client_now->socket, "你不在群组中", strlen("你不在群组中"), 0);
				break;
			}
			
			if(send_group(client_now, group_id, tmp.c_str(), tmp.size()))
				send(client_now->socket, "发送成功", strlen("发送成功"), 0);
			else
				send(client_now->socket, "发送失败", strlen("发送失败"), 0);
			
			break;

			// 列出在线的群
		case 4:
			if (group_idx == -1) tmp = "没有群在线!";
			for (int i = 0; i <= group_idx; i ++)
				tmp += to_string(i) + "." + groups[i] + "\n";
			send(client_now->socket, tmp.c_str(), tmp.size(), 0);
			break;

			// 加入某个群
		case 8:
			group_id = atoi(packet->msg);
			if (limit[group_id])
			{
				send(client_now->socket, "加入失败，该群限制加入", strlen("加入失败，该群限制加入"), 0);
				break;
			}
			if (join_group(client_now, group_id))
				send(client_now->socket, "加入成功", strlen("加入成功"), 0);
			else send(client_now->socket, "加入失败，已在该群中", strlen("加入失败，已在该群中"), 0);
			break;

			// 创建一个群
		case 16:
			groups.push_back(packet->msg);
			group_idx ++;
			send(client_now->socket, "创建成功", strlen("创建成功"), 0);
			break;

			// 设置群为限制加入
		case 32:
			group_id = atoi(packet->msg);
			limit[group_id] = 1;
			send(client_now->socket, "设置成功", strlen("设置成功"), 0);
			break;

			// 退出某个群聊
		case 64:
			group_id = atoi(packet->msg);
			if (quit_group(client_now, group_id))
				send(client_now->socket, "退出成功", strlen("退出成功"), 0);
			else
				send(client_now->socket, "退出失败， 你不在该群中", strlen("退出失败， 你不在该群中"), 0);
			break;

			// 退出程序
		case 128:
			closesocket(client_now->socket);
			delete client_now;
			return 0;

			//丢弃非CCP协议数据包
		default:
			break;
		}
	}
}

int main()
{
	// 初始化socket	
	if (!start_socket()) cout << "socket alreay open!" << endl;
	else return -1;

	// 服务端socket 
	SOCKET serSocket = socket(AF_INET, SOCK_STREAM, 0);

	// 设置非阻塞socket
	// int iMode = 1;
	// ioctlsocket(serSocket,FIONBIO, reinterpret_cast<u_long*>(&iMode));

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
		HANDLE deal_thread = CreateThread(nullptr, 0, deal_client, reinterpret_cast<LPVOID>(client), 0, &ThreadId);
		CloseHandle(deal_thread);
	}

}
