#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <string>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

int main()
{
	// 初始化socket
	// 开启socket版本检查
	WSADATA wsaData;
	const auto versionRequired = MAKEWORD(2, 2);
	if(!WSAStartup(versionRequired, &wsaData)) cout << "socket alreay open!" << endl;
	else return -1;

	// 发送端socket 
	SOCKET send_Socket = socket(AF_INET, SOCK_DGRAM, 0);

	// 设置接收端socket地址
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(2333);

	//绑定地址和接收端socket
	// if(bind(send_Socket,(SOCKADDR*)&addr,sizeof(SOCKADDR))<0)
	// {
	// 	cout << "套接字描述符与套接字地址绑定失败!" << endl;
	// 	// 关闭socket
	// 	closesocket(send_Socket);
	// 	WSACleanup();
	// 	return 0;
	// }

	// sendto
	char buf[80];
	
	sprintf(buf,"data packet with ID 1\n");
	
	//目地机的IP地址和端口号信息
	sendto(send_Socket, buf,strlen(buf),0,(struct sockaddr *)&addr,sizeof(addr));
	printf("本机往服务器端发送的数据为:\n%s\n",buf);
	while(1)
	{
		char recvstr[80];
		int len=sizeof(addr);
		int recvnum=recvfrom(send_Socket,recvstr,80,0,(struct sockaddr *)&addr,&len);
		if(recvnum>0)
		{
			recvstr[recvnum]='\0';
			printf("服务器端发来的数据为:\n%s\n",recvstr);
		}
	}


	closesocket(send_Socket);
	WSACleanup();
	return 0;
}