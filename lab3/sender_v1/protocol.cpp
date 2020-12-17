#include "protocol.h"

// 计算校验和
bool check_packet(packet * new_packet)
{
	unsigned short result = 0;
	result += new_packet->FIN + new_packet->PSH + new_packet->SYN + new_packet->size + new_packet->seq;
	for (unsigned int i = 0; i < new_packet->size; i ++)
	{
		result += (unsigned short)(*(new_packet->data + i));
	}
	result += new_packet->checksum + 1;
	return (result == 0);
}

// 封装数据包
packet * make_packet(unsigned int syn, unsigned int psh, unsigned int fin,
	unsigned int size, unsigned int seq, const char* data)
{
	packet * new_packet = (packet *)malloc(sizeof(packet) + size + 1);
	new_packet->SYN = syn;
	new_packet->PSH = psh;
	new_packet->FIN = fin;
	new_packet->size = size;
	new_packet->seq = seq;

	if (data != NULL)
	{
		for (int i = 0; i < size; i ++)
		{
			new_packet->data[i] = data[i];
		}
		new_packet->data[size] = '\0';
	}

	// 计算校验和
	unsigned short result = 0;
	result += new_packet->FIN + new_packet->PSH + new_packet->SYN + new_packet->size + new_packet->seq;
	for (unsigned int i = 0; i < size; i ++)
	{
		result += (unsigned short)(*(new_packet->data + i));
	}
	new_packet->checksum = ~result;
	return new_packet;
}