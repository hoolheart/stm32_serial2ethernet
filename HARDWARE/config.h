#ifndef __CONFIG_H__
#define __CONFIG_H__
#include "stm32f4xx.h"
#include "uart_fsmc.h"

/** system configuration */
typedef __packed struct strSysConfig {
	u8 header[2];/**< header: 0xaa */
	u8 ip_addr[4];/**< local IP address */
	u8 ip_mask[4];/**< local IP mask */
	u8 ip_gate[4];/**< gate IP */
	
	u16 listen_port;/**< listening port */
	u8  remote_ip[4];/**< remote IP address */
	u16 remote_port;/**< remote port */
	
	u32 baud_rates[UART_FSMC_NUM];/**< baud rates */
	u8  parities[UART_FSMC_NUM];/**< parities */
	
	u8  query_cycle_s;/**< query cycle in s */
}SYS_CONFIG;

/** prepare system configuration */
void prepareConfig(void);
/** get system configuration */
SYS_CONFIG * getConfig(void);
/** set system configuration */
void setConfig(SYS_CONFIG n_cfg);
/** apply configuration by reboot */
void applyConfig(void);

#endif //__CONFIG_H__
