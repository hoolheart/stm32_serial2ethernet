#include "uart_fsmc.h"
#include "fsmc.h"
#include "delay.h"
#include "config.h"

#define RESET_ADDR 0
#define UART1_BASE 0X2000
#define BASE_INTERVAL 0X100

/** initialize a channel of fsmc-driven uart */
void initUartChlFsmc(u8 chl, u32 baud_rate, u8 parity) {
	//prepare base address
	u32 addr_base = UART1_BASE+BASE_INTERVAL*chl;
	//prepare baud rate factor
	u16 baud_factor = (u16)(14745600/baud_rate);
	//check channel
	if(chl<UART_FSMC_NUM) {
		//set baud rate
		writeFSMC(addr_base,(u8)(baud_factor&0xff));
		writeFSMC(addr_base+1,(u8)(baud_factor>>8));
		//set parity
		if(parity==0) {
			//no parity
			writeFSMC(addr_base+2,0xc0);
		}
		else if(parity==1) {
			//odd
			writeFSMC(addr_base+2,0xc1);
		}
		else {
			//even
			writeFSMC(addr_base+2,0xd1);
		}
	}
}

/** initialize uart from fsmc interface */
void initUartFsmc() {
	u8 chl;
	
	//get config
	SYS_CONFIG *cfg = getConfig();
	
	resetUartFsmc();//reset first
	//initial all channels
	for(chl=0;chl<UART_FSMC_NUM;chl++) {
		initUartChlFsmc(chl,cfg->baud_rates[chl],cfg->parities[chl]);
	}
}

/** reset uart */
void resetUartFsmc() {
	writeFSMC(RESET_ADDR,0);
	delay_us(100);
	writeFSMC(RESET_ADDR,1);
}

/** send data to fsmc-driven uart */
u8 sendDataUartFsmc(u8 chl,u8 *ptr,u8 size) {
	u8 i;
	//prepare base address
	u32 addr_base = UART1_BASE+BASE_INTERVAL*chl;
	//prepare send count
	u8  pre_size = readFSMC(addr_base+6);
	u8  post_size;
	//check channel
	if((chl<UART_FSMC_NUM) && (size>0)) {
		for(i=0;i<size;i++) {
			writeFSMC(addr_base+4,ptr[i]);//write
		}
		post_size = readFSMC(addr_base+6);
		if(post_size>pre_size) {
			return post_size-pre_size;
		}
		else {
			return 256+post_size-pre_size;
		}
	}
	return 0;
}

/** size of pending recerved data of fsmc-driven uart */
u8 pendingNumUartFsmc(u8 chl) {
	//prepare base address
	u32 addr_base = UART1_BASE+BASE_INTERVAL*chl;
	//check channel
	if(chl<UART_FSMC_NUM) {
		return readFSMC(addr_base+7);
	}
	return 0;
}

/** receive data from fsmc-driven uart */
u8 receiveDataUartFsmc(u8 chl,u8 *ptr,u8 max) {
	u8 i;
	//prepare base address
	u32 addr_base = UART1_BASE+BASE_INTERVAL*chl;
	//prepare send count
	u8  pre_size;
	//check channel
	if((chl<UART_FSMC_NUM) && (max>0)) {
		pre_size = readFSMC(addr_base+7);//get pending size
		for(i=0;(i<max) && (i<pre_size);i++) {
			ptr[i] = readFSMC(addr_base+4);//read data
		}
		return i;
	}
	return 0;
}
