#pragma once

/* 聊天协议 CCP(Chat Control Protocol)
 *
 * 状态码	功能                客户端指令
 * 10000	列出在线的群			list
 * 11000	加入某个群			join <group_id>
 * 11100	创建一个群			create <group_name>
 * 11110	设置群为限制加入		limit
 * 11111	退出					quit
 * 00000	发送消息
 * 
 * 标志		客户端		服务端
 * 01		开始	发送 | 准备接受
 * 11		正在发送 | 接受成功
 * 10		发送结束 | 接受完毕 
 * 
*/
typedef struct chat_packet
{
	// 状态码(5位功能码+2位标志+1位奇偶校验码)
	byte state_code;

	// 消息长度
	int msg_len;

	// 消息
	char *msg;
	
}chat_packet;

// 封包
chat_packet * pack_packet()
{
	
}

// 解包
chat_packet * unpack_packet()
{
	
}