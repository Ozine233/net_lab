#pragma once
#include "protocol.h"
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <memory>
#define	MAX_BUF 3000
#define PORT unsigned short

#pragma comment(lib,"ws2_32.lib")

bool net_init();

SOCKET get_socket(SOCKADDR_IN *addr);

void del_socket(SOCKET d_socket);

SOCKADDR_IN * get_addr(std::string ip, PORT port);

void set_addr(SOCKADDR_IN *addr, std::string ip, PORT port);

void del_addr(SOCKADDR_IN *addr);

int send_packet(SOCKET s_socket, SOCKADDR_IN *addr, packet * s_packet);

int recv_packet(SOCKET r_socket, packet * r_packet, SOCKADDR_IN *addr, int &len);

void net_close();