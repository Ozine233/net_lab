#include "socket.h"
#include <exception>

#pragma comment(lib,"ws2_32.lib")

socket::socket(string &ip, int port)
{
	// ��ʼ��socket
	// ����socket�汾���
	WSADATA wsaData;
	const auto versionRequired = MAKEWORD(2, 2);
	if (!WSAStartup(versionRequired, &wsaData))
}

socket::~socket()
{
}