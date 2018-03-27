#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

//ÿ��UDP��Ϣ����4����:
//1. 2�ֽڵı���ͷ: 0xaa 0xaa
//2. MESSAGE_HEAD
//3. ��Ϣ����
//4. У���

//ע��:
//1. MESSAGE_HEAD�еĳ��Ȱ���MESSAGE_HEAD����Ϣ���ĺ�У��ͣ���������2�ֽڵı���ͷ
//2. У��ͼ��㷽��ΪMESSAGE_HEAD����Ϣ����ȫ���ֽ�֮��ȡ����1����MESSAGE_HEAD����Ϣ���ĺ�У���������ȫ���ֽ�֮��Ϊ0

//���ܺ�:
//1. 0xA1 ����λ���������Ĵ������ݣ���ӦCOM_DEV_UART_DATA
//2. 0xB1 �ռ����ϱ�����λ���Ĵ������ݣ���ӦDEV_COM_UART_REPORT
//3. 0xA2 ��λ���д���ͨ�� (��Ϣ����Ϊ��)
//4. 0xA3 �������ã���������Ч������ӦCOM_DEV_CONFIG

#progma pack(push)
#progma pack(1)

typedef struct {
	u16 len;//���ĳ���
	u16 code;//���ܺ�
}MESSAGE_HEAD;

#define MAX_UART_DATA_SIZE 255

typedef struct {
	MESSAGE_HEAD head;
	u8 uart_chl;//����ͨ���� 0-15
	u8 num;//�������ݳ��� 1-255
	u8 data[MAX_UART_DATA_SIZE];//��������
}COM_DEV_UART_DATA;

typedef struct {
	MESSAGE_HEAD head;
	u8 uart_chl;//����ͨ���� 0-15
	u8 num;//�������ݳ��� 1-255
	u8 data[MAX_UART_DATA_SIZE];//��������
}DEV_COM_UART_REPORT;

typedef struct {
	MESSAGE_HEAD head;
	
	u8 ip_addr[4];/**< ����IP��ַ */
	u8 ip_mask[4];/**< �������� */
	u8 ip_gate[4];/**< ����IP��ַ */
	
	u16 listen_port;/**< �����˿� */
	u8  remote_ip[4];/**< �ϱ�IP��ַ */
	u16 remote_port;/**< �ϱ��˿� */
	
	u32 baud_rates[16];/**< 16��ͨ���Ĳ����� */
	u8  parities[16];/**< 16��ͨ����У�鷽ʽ��0Ϊ��У�飬1Ϊ��У�飬2ΪżУ�� */
	
	u8  query_cycle_s;/**< ��ѯ���ڣ���λs */
}COM_DEV_CONFIG;

#progma pack(pop)

#endif //__PROTOCOL_H__
