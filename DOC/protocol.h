#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

//每个UDP消息包括4部分:
//1. 2字节的报文头: 0xaa 0xaa
//2. MESSAGE_HEAD
//3. 信息正文
//4. 校验和

//注意:
//1. MESSAGE_HEAD中的长度包括MESSAGE_HEAD、信息正文和校验和，但不包括2字节的报文头
//2. 校验和计算方法为MESSAGE_HEAD和信息正文全部字节之和取反加1，即MESSAGE_HEAD、信息正文和校验和三部分全部字节之和为0

//功能号:
//1. 0xA1 从上位机发送来的串口数据，对应COM_DEV_UART_DATA
//2. 0xB1 收集来上报到上位机的串口数据，对应DEV_COM_UART_REPORT
//3. 0xA2 复位所有串口通道 (消息正文为空)
//4. 0xA3 更改配置（重启后生效），对应COM_DEV_CONFIG

#progma pack(push)
#progma pack(1)

typedef struct {
	u16 len;//报文长度
	u16 code;//功能号
}MESSAGE_HEAD;

#define MAX_UART_DATA_SIZE 255

typedef struct {
	MESSAGE_HEAD head;
	u8 uart_chl;//串口通道号 0-15
	u8 num;//串口数据长度 1-255
	u8 data[MAX_UART_DATA_SIZE];//串口数据
}COM_DEV_UART_DATA;

typedef struct {
	MESSAGE_HEAD head;
	u8 uart_chl;//串口通道号 0-15
	u8 num;//串口数据长度 1-255
	u8 data[MAX_UART_DATA_SIZE];//串口数据
}DEV_COM_UART_REPORT;

typedef struct {
	MESSAGE_HEAD head;
	
	u8 ip_addr[4];/**< 本地IP地址 */
	u8 ip_mask[4];/**< 子网掩码 */
	u8 ip_gate[4];/**< 网关IP地址 */
	
	u16 listen_port;/**< 监听端口 */
	u8  remote_ip[4];/**< 上报IP地址 */
	u16 remote_port;/**< 上报端口 */
	
	u32 baud_rates[16];/**< 16个通道的波特率 */
	u8  parities[16];/**< 16个通道的校验方式，0为无校验，1为奇校验，2为偶校验 */
	
	u8  query_cycle_s;/**< 轮询周期，单位s */
}COM_DEV_CONFIG;

#progma pack(pop)

#endif //__PROTOCOL_H__
