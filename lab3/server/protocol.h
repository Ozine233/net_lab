#pragma once
#include <cstdlib>
#include <cstring>
#include <xutility>
/*
 * ����UDP�Ŀɿ�����Э�� V1.0
 * SYN �������ӱ�־λ
 * PSH �������ݱ�־λ
 * FIN ���������־λ
 * size ���ݶδ�С
 * checksum У���
 * seq ���ݰ����
 * data ���ݰ�Я��������
 */

typedef struct packet
{
	unsigned int SYN : 1;
	unsigned int PSH : 1;
	unsigned int FIN : 1;
	unsigned int size : 13;
	unsigned int checksum : 16;
	unsigned int seq;

	char data[0];
	
}packet;

// ����У���
bool check_packet(packet * new_packet)
{
	unsigned short result = 0;
	result += new_packet->FIN + new_packet->PSH + new_packet->SYN + new_packet->size + new_packet->seq;
	for(unsigned int i = 0; i < new_packet->size; i ++)
	{
		result += (unsigned short)(*(new_packet->data + i));
	}
	result += new_packet->checksum + 1;
	return (result == 0);
}
