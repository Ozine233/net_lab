#pragma once
#include <cstdlib>
#include <cstring>
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

// ��װ���ݰ�
packet * make_packet(unsigned int syn, unsigned int psh, unsigned int fin,
	unsigned int size, unsigned int seq, const char* data)
{
	packet * new_packet = (packet *)malloc(sizeof packet + size + 1);
	new_packet->SYN = syn;
	new_packet->PSH = psh;
	new_packet->FIN = fin;
	new_packet->size = size;
	new_packet->seq = seq;
	strcpy_s(new_packet->data, size + 1, data);

	// ����У���
	unsigned short result = 0;
	result += new_packet->FIN + new_packet->PSH + new_packet->SYN + new_packet->size + new_packet->seq;
	for(unsigned int i = 0; i < size; i ++)
	{
		result += (unsigned short)(*(new_packet->data + i));
	}
	new_packet->checksum = ~result;
	return new_packet;
}
