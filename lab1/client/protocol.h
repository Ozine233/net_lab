#pragma once
#include <malloc.h>

/* ����Э�� CCP(Chat Control Protocol)
 *
 * ״̬��		����                �ͻ���ָ��
 * 00000001		ע������				register <nick_name>
 * 00000010		������Ϣ				send <group_id> <message>
 * 00000100		�г����ߵ�Ⱥ			list
 * 00001000		����ĳ��Ⱥ			join <group_id>
 * 00010000		����һ��Ⱥ			create <group_name>
 * 00100000		����ȺΪ���Ƽ���		limit <group_id>
 * 01000000		�˳�ĳ��Ⱥ��			quit <group_id> 
 * 10000000		�˳�					exit
 * 
*/
typedef struct chat_packet
{
	// ״̬��(6λ������+2λ��־)
	byte state_code;

	// ��Ϣ����
	byte msg_len;

	// ��Ϣ
	char msg[0];
}chat_packet;


// ���
inline chat_packet * make_packet(const byte code, int len, const char *msg)
{
	chat_packet *packet = static_cast<chat_packet*>(malloc(sizeof(chat_packet) + len + 1));
	packet->state_code = code;
	packet->msg_len = static_cast<byte>(len);
	strcpy_s(packet->msg, len + 1, msg);
	return packet;
}