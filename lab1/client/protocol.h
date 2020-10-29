#pragma once
#include <malloc.h>

/* 聊天协议 CCP(Chat Control Protocol)
 *
 * 状态码		功能                客户端指令
 * 00000001		注册名字				register <nick_name>
 * 00000010		发送消息				send <group_id> <message>
 * 00000100		列出在线的群			list
 * 00001000		加入某个群			join <group_id>
 * 00010000		创建一个群			create <group_name>
 * 00100000		设置群为限制加入		limit <group_id>
 * 01000000		退出某个群聊			quit <group_id> 
 * 10000000		退出					exit
 * 
*/
typedef struct chat_packet
{
	// 状态码(6位功能码+2位标志)
	byte state_code;

	// 消息长度
	byte msg_len;

	// 消息
	char msg[0];
}chat_packet;


// 封包
inline chat_packet * make_packet(const byte code, int len, const char *msg)
{
	chat_packet *packet = static_cast<chat_packet*>(malloc(sizeof(chat_packet) + len + 1));
	packet->state_code = code;
	packet->msg_len = static_cast<byte>(len);
	strcpy_s(packet->msg, len + 1, msg);
	return packet;
}