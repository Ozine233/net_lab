#pragma once

/* ����Э�� CCP(Chat Control Protocol)
 *
 * ״̬��	����                �ͻ���ָ��
 * 10000	�г����ߵ�Ⱥ			list
 * 11000	����ĳ��Ⱥ			join <group_id>
 * 11100	����һ��Ⱥ			create <group_name>
 * 11110	����ȺΪ���Ƽ���		limit
 * 11111	�˳�					quit
 * 00000	������Ϣ
 * 
 * ��־		�ͻ���		�����
 * 01		��ʼ	���� | ׼������
 * 11		���ڷ��� | ���ܳɹ�
 * 10		���ͽ��� | ������� 
 * 
*/
typedef struct chat_packet
{
	// ״̬��(5λ������+2λ��־+1λ��żУ����)
	byte state_code;

	// ��Ϣ����
	int msg_len;

	// ��Ϣ
	char *msg;
	
}chat_packet;

// ���
chat_packet * pack_packet()
{
	
}

// ���
chat_packet * unpack_packet()
{
	
}