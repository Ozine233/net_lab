#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <string>
#include "protocol.h"

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

	// 接收端socket 
	SOCKET recv_Socket = socket(AF_INET, SOCK_DGRAM, 0);

	// 设置接收端socket地址
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(2333);

	// 绑定地址和接收端socket
	if(bind(recv_Socket,(SOCKADDR*)&addr,sizeof(SOCKADDR))<0)
	{
		cout << "套接字描述符与套接字地址绑定失败!" << endl;
		// 关闭socket
		closesocket(recv_Socket);
		WSACleanup();
		return 0;
	}

	while(1)
	{
		char recvdata[80] = {0};
		packet * mypacket = nullptr;
		int len=sizeof(addr);
		const char *s2cData="hello,client,this is the server!\n";
	    //recvfrom( SOCKET s, char FAR* buf, int len, int flags,struct sockaddr FAR* from, int FAR* fromlen);
		//s：标识一个已连接套接口的描述字。
		//buf：接收数据缓冲区。
		//len：缓冲区长度。
		//flags：调用操作方式。
		//from：（可选）指针，指向装有源地址的缓冲区。
		//fromlen：（可选）指针，指向from缓冲区长度值。
		//int recvnum=recvfrom(recv_Socket,recvdata,strlen(recvdata),0,(struct sockaddr *)&addr,&len);
		
		int recvnum=recvfrom(recv_Socket,recvdata,80,0,(struct sockaddr *)&addr,&len);
		if(recvnum>0)
		{
			mypacket = (packet *)recvdata;
			if(check_packet(mypacket))
			{
				cout << "校验成功" << endl;
				for(int i = 0; i < mypacket->size; i ++)
					cout << *(mypacket->data + i);
				cout << endl;
			}
			else
			{
				cout << "校验失败" << endl;
			}
		}
		//服务器端收到数据以后，要客户端回送响应 "hello，client,this is server!"
		sendto(recv_Socket,s2cData,strlen(s2cData),0,(struct sockaddr *)&addr,sizeof(addr));
		printf("向客户端发送的数据为\n%s\n",s2cData);
 
	}

	

	closesocket(recv_Socket);
	WSACleanup();
	return 0;
}
