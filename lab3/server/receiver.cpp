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
	// ��ʼ��socket
	// ����socket�汾���
	WSADATA wsaData;
	const auto versionRequired = MAKEWORD(2, 2);
	if(!WSAStartup(versionRequired, &wsaData)) cout << "socket alreay open!" << endl;
	else return -1;

	// ���ն�socket 
	SOCKET recv_Socket = socket(AF_INET, SOCK_DGRAM, 0);

	// ���ý��ն�socket��ַ
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(2333);

	// �󶨵�ַ�ͽ��ն�socket
	if(bind(recv_Socket,(SOCKADDR*)&addr,sizeof(SOCKADDR))<0)
	{
		cout << "�׽������������׽��ֵ�ַ��ʧ��!" << endl;
		// �ر�socket
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
		//s����ʶһ���������׽ӿڵ������֡�
		//buf���������ݻ�������
		//len�����������ȡ�
		//flags�����ò�����ʽ��
		//from������ѡ��ָ�룬ָ��װ��Դ��ַ�Ļ�������
		//fromlen������ѡ��ָ�룬ָ��from����������ֵ��
		//int recvnum=recvfrom(recv_Socket,recvdata,strlen(recvdata),0,(struct sockaddr *)&addr,&len);
		
		int recvnum=recvfrom(recv_Socket,recvdata,80,0,(struct sockaddr *)&addr,&len);
		if(recvnum>0)
		{
			mypacket = (packet *)recvdata;
			if(check_packet(mypacket))
			{
				cout << "У��ɹ�" << endl;
				for(int i = 0; i < mypacket->size; i ++)
					cout << *(mypacket->data + i);
				cout << endl;
			}
			else
			{
				cout << "У��ʧ��" << endl;
			}
		}
		//���������յ������Ժ�Ҫ�ͻ��˻�����Ӧ "hello��client,this is server!"
		sendto(recv_Socket,s2cData,strlen(s2cData),0,(struct sockaddr *)&addr,sizeof(addr));
		printf("��ͻ��˷��͵�����Ϊ\n%s\n",s2cData);
 
	}

	

	closesocket(recv_Socket);
	WSACleanup();
	return 0;
}
