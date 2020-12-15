#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <string>
#include "protocol.h"

using namespace std;

class socket
{
public:
	socket(string &ip, int port);
	~socket();
	
private:
	SOCKADDR_IN addr;
	SOCKET SendSocket;
};

