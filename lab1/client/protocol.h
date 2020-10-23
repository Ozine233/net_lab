#pragma once

/* ����Э�� CCP(Chat Control Protocol)
 *
 * ״̬��		����                �ͻ���ָ��
 * 010000		ע������				register <nick_name>
 * 010001		������Ϣ				send <group_id> <message>
 * 010010		�г����ߵ�Ⱥ			list
 * 010011		����ĳ��Ⱥ			join <group_id>
 * 010100		����һ��Ⱥ			create <group_name>
 * 010101		����ȺΪ���Ƽ���		limit
 * 010110		�˳�ĳ��Ⱥ��			quit <group_id> 
 * 010111		�˳�					exit
 * 
 * 
 * ��־		�ͻ���		�����
 * 01		��ʼ	���� | ׼������
 * 11		���ڷ��� | ���ܳɹ�
 * 10		���ͽ��� | ������� 
 * 00		һ����ָ��
*/
typedef struct chat_packet
{
	// ״̬��(6λ������+2λ��־)
	byte state_code;

	// ��Ϣ����
	int msg_len;

	// ��Ϣ
	const char *msg;
	
}chat_packet;


// ���
inline chat_packet * make_packet(const byte code, int len, const char *msg)
{
	chat_packet *packet = new chat_packet;
	packet->state_code = code;
	packet->msg_len = len;
	packet->msg = msg;
	return packet;
}