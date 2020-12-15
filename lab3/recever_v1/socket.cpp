#include "socket.h"

// ��ʼ��
// ���socket�汾
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

// ��socket �� ��ַ
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

// ɾ��socket
void del_socket(SOCKET d_socket)
{
	closesocket(d_socket);
}

// ��õ�ַ
SOCKADDR_IN * get_addr(std::string ip, PORT port)
{
	SOCKADDR_IN *addr = (SOCKADDR_IN *)malloc(sizeof(SOCKADDR_IN));
	addr->sin_family = AF_INET;
	addr->sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	addr->sin_port = htons(port);

	return addr;
}

// �ı��ַ
void set_addr(SOCKADDR_IN *addr, std::string ip, PORT port)
{
	addr->sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	addr->sin_port = htons(port);
}

// ɾ����ַ
void del_addr(SOCKADDR_IN *addr)
{
	if (NULL != addr)
		free(addr);
}

// ���� packet
int send_packet(SOCKET s_socket, SOCKADDR_IN *addr, packet * s_packet)
{
	return sendto(s_socket, (char *)s_packet, sizeof(packet) + s_packet->size + 1, 0, (struct sockaddr *)addr, sizeof(*addr));
}

// ����packet
int recv_packet(SOCKET r_socket, packet * r_packet, SOCKADDR_IN *addr, int &len)
{
	return recvfrom(r_socket, (char *)r_packet, MAX_BUF, 0, (struct sockaddr *)addr, &len);
}

// ����ͨ��
void net_close()
{
	WSACleanup();
}