/**
  ******************************************************************************
  * @file    main.c
  * @author  Zhou Huide
  * @version V1.0
  * @date    24-August-2016
  * @brief   Main function for multi-CAN interface board
  ******************************************************************************
*/


#include "stm32f4xx.h"
#include "core_cm4.h"
#include "wdg.h"
#include "msg_interface.h"
#include "msg_code_hardware.h"
#include "msg_code_user.h"
#include "msg_handlers.h"
#include "console.h"
#include "debug_led.h"
#include "delay.h"
#include "stdio.h"
#include "fsmc.h"
#include "timer.h"
#include "lwip_comm.h"
#include "LAN8720.h"
#include "malloc.h"
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"

#include "config.h"
#include "uart_fsmc.h"
#include "udp_interface.h"
#include "protocol_layer.h"

#include "httpd.h"

void initilize() {
	delay_init(168);//delay engine
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	initMsgInterface();//message interface
	initConsole(115200);//console
	mymem_init(SRAMIN);		//initialize internal momery pool
	mymem_init(SRAMCCM);	//initialize internal CCM pool
	prepareConfig();			//prepare configuration

	initDebugLED();//LED
	//flash LED
	openAllLED();
	closeLED(4);
	delay_ms(3000);
	closeAllLED();
	delay_ms(2000);
	
	//initialize lwip
	while(lwip_comm_init()!=0) {
		printf("lwIP Init failed! Retrying...");
		delay_ms(1200);
	}

	openLED(0);
	printf("Hello, this is NET-RS232 interface.\r\n");
	initTimer();//timer

	initFSMC();//FSMC
	initUartFsmc();				//initialize fsmc-driven uart
	initProtocolLayer();
	prepareUDPConnection(onRecvData);

	httpd_init();
}

void blink() {
	u8 i;
	for(i=0;i<5;i++) {
		closeLED(5);
		delay_ms(200);
		openLED(5);
		delay_ms(200);
	}
	closeLED(5);
}

void processMsg() {
	int msg,para;//prepare data
	if(getNextMsg(&msg,&para)==0) {
		switch(msg) {
		case MSG_CONSOLE_COMMAND:
			onConsoleCommand(para);
			break;
		case MSG_WORKTICK:
			onWorkTick(para);
			break;
		case MSG_CONFIG_APPLY:
			blink();
		case MSG_LINK_DOWN:
			writeFSMC(1,0);
			writeFSMC(1,1);
			writeFSMC(1,0);
			break;
		default:
			break;
		}
	}
}

int main(void) {
	initilize();

	while(1) {
		processMsg();
		lwip_periodic_handle();
	}
}
