#pragma once
#include <cstdlib>
#include <cstring>
#include <xutility>
/*
 * 基于UDP的可靠传输协议 V1.0
 * SYN 建立连接标志位
 * PSH 传送数据标志位
 * FIN 传输结束标志位
 * size 数据段大小
 * checksum 校验和
 * seq 数据包序号
 * data 数据包携带的数据
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

// 计算校验和
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
