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

bool check_packet(packet * new_packet);

packet * make_packet(unsigned int syn, unsigned int psh, unsigned int fin, unsigned int size, unsigned int seq, const char* data);