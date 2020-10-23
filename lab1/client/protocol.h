#pragma once

/* 聊天协议 CCP(Chat Control Protocol)
 *
 * 状态码		功能                客户端指令
 * 010000		注册名字				register <nick_name>
 * 010001		发送消息				send <group_id> <message>
 * 010010		列出在线的群			list
 * 010011		加入某个群			join <group_id>
 * 010100		创建一个群			create <group_name>
 * 010101		设置群为限制加入		limit
 * 010110		退出某个群聊			quit <group_id> 
 * 010111		退出					exit
 * 
 * 
 * 标志		客户端		服务端
 * 01		开始	发送 | 准备接受
 * 11		正在发送 | 接受成功
 * 10		发送结束 | 接受完毕 
 * 00		一次性指令
*/
typedef struct chat_packet
{
	// 状态码(6位功能码+2位标志)
	byte state_code;

	// 消息长度
	int msg_len;

	// 消息
	const char *msg;
	
}chat_packet;


// 封包
inline chat_packet * make_packet(const byte code, int len, const char *msg)
{
	chat_packet *packet = new chat_packet;
	packet->state_code = code;
	packet->msg_len = len;
	packet->msg = msg;
	return packet;
}