#include "socket.h"

// 初始化
// 检查socket版本
bool net_init()
{
	const auto VersionRequired = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (!WSAStartup(VersionRequired, &wsaData))
	{
		return true;
	}
	return false;
}

// 绑定socket 和 地址
SOCKET get_socket(SOCKADDR_IN *addr)
{
	SOCKET r_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (bind(r_socket, (SOCKADDR *)addr, sizeof(SOCKADDR)) < 0)
	{
		closesocket(r_socket);
		return NULL;
	}
	return r_socket;
}

// 删除socket
void del_socket(SOCKET d_socket)
{
	closesocket(d_socket);
}

// 获得地址
SOCKADDR_IN * get_addr(std::string ip, PORT port)
{
	SOCKADDR_IN *addr = (SOCKADDR_IN *)malloc(sizeof(SOCKADDR_IN));
	addr->sin_family = AF_INET;
	addr->sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	addr->sin_port = htons(port);

	return addr;
}

// 改变地址
void set_addr(SOCKADDR_IN *addr, std::string ip, PORT port)
{
	addr->sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	addr->sin_port = htons(port);
}

// 删除地址
void del_addr(SOCKADDR_IN *addr)
{
	if (NULL != addr)
		free(addr);
}

// 发送 packet
int send_packet(SOCKET s_socket, SOCKADDR_IN *addr, packet * s_packet)
{
	return sendto(s_socket, (char *)s_packet, sizeof(packet) + s_packet->size + 1, 0, (struct sockaddr *)addr, sizeof(*addr));
}

// 接受packet
int recv_packet(SOCKET r_socket, packet * r_packet, SOCKADDR_IN *addr, int &len)
{
	return recvfrom(r_socket, (char *)r_packet, MAX_BUF, 0, (struct sockaddr *)addr, &len);
}

// 结束通信
void net_close()
{
	WSACleanup();
}