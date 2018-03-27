#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__
#include "stm32f4xx.h"

//every message includes 4 parts:
//1. 2 header bytes: 0xaa 0xaa
//2. MESSAGE_HEAD
//3. message content
//4. check sum (CS)

//note:
//1. length in MESSAGE_HEAD includes MESSAGE_HEAD, content and CS
//2. CS is given by making sum of all bytes in MESSAGE_HEAD, content and CS is 0

//message code:
//1. 0xA1 uart data sent to sub-devices from ethernet
//2. 0xB1 uart data sent by sub-devices to ethernet
//3. 0xA2 reset all uart channels (no content)
//4. 0xA3 set config

typedef __packed struct {
	u16 len;//message length (entire message without 2 header bytes)
	u16 code;//message code
}MESSAGE_HEAD;

#define MAX_UART_DATA_SIZE 255

typedef __packed struct {
	MESSAGE_HEAD head;
	u8 uart_chl;//channel of uart 0-15
	u8 num;//number of data to send 1-255
	u8 data[MAX_UART_DATA_SIZE];//data
}COM_DEV_UART_DATA;

typedef __packed struct {
	MESSAGE_HEAD head;
	u8 uart_chl;//channel of uart 0-15
	u8 num;//number of data to send 1-255
	u8 data[MAX_UART_DATA_SIZE];//data
}DEV_COM_UART_REPORT;

typedef __packed struct {
	MESSAGE_HEAD head;
	
	u8 ip_addr[4];/**< local IP address */
	u8 ip_mask[4];/**< local IP mask */
	u8 ip_gate[4];/**< gate IP */
	
	u16 listen_port;/**< listening port */
	u8  remote_ip[4];/**< remote IP address */
	u16 remote_port;/**< remote port */
	
	u32 baud_rates[16];/**< baud rates */
	u8  parities[16];/**< parities */
	
	u8  query_cycle_s;/**< query cycle in s */
}COM_DEV_CONFIG;

#endif //__PROTOCOL_H__
