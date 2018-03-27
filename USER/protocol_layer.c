#include "protocol.h"
#include "protocol_layer.h"
#include "udp_interface.h"
#include "config.h"
#include "uart_fsmc.h"
#include "malloc.h"
#include "debug_led.h"

u8 udp_buff[1024];
u8 step=0;//0 wait_head1, 1 wait_head2, 2 len1, 3 len2, 4 content, 5 cs
u16 udp_len=0;
u32 index_buff=0;

u8 *report_buff=0;
DEV_COM_UART_REPORT *report_ptr=0;

/** initialize protocol layer */
void initProtocolLayer(void) {
	step = 0;
	index_buff = 0;
	if(report_buff==0) {
		report_buff = mymalloc(SRAMIN,sizeof(DEV_COM_UART_REPORT)+3);
		if(report_buff) {
			mymemset(report_buff,0,sizeof(DEV_COM_UART_REPORT)+3);
			report_buff[0] = 0xaa; report_buff[1] = 0xaa;//head
			report_ptr = (DEV_COM_UART_REPORT*)(report_buff+2);//message pointer
			report_ptr->head.code = 0xB1;//message code
			report_ptr->head.len  = sizeof(DEV_COM_UART_REPORT)+1;//length
		}
	}
}

/** send uart */
void sendUart(u8* ptr) {
	COM_DEV_UART_DATA *msg = (COM_DEV_UART_DATA*)ptr;//apply protocol
	if(msg->head.len>=(msg->num+2+sizeof(MESSAGE_HEAD)+1)) {//check size
		sendDataUartFsmc(msg->uart_chl,msg->data,msg->num);
		openLED(3);
	}
}

/** check uart */
u8 checkUartFsmc(u8 chl) {
	u8 size=0,cs;
	u32 i;
	if((chl<UART_FSMC_NUM) && report_ptr) {
		report_ptr->uart_chl = chl;//channel
		size = receiveDataUartFsmc(chl,report_ptr->data,MAX_UART_DATA_SIZE);//data
		if(size>0) {
			report_ptr->num = size;//size
			report_ptr->head.len  = sizeof(DEV_COM_UART_REPORT)+1-(MAX_UART_DATA_SIZE-size);//length
			cs = 0;
			for(i=2;i<(report_ptr->head.len+2-1);i++) {
				cs += report_buff[i];
			}
			report_buff[report_ptr->head.len+2-1] = 0-cs;//check sum
			sendUdpDatagram(report_buff,report_ptr->head.len+2);//send udp datagram
			openLED(2);
		}
	}
	return size;
}

/** test uart */
void testUart(void) {
	u8 size=15,cs;
	u32 i;
	report_ptr->uart_chl = 7;//channel
	report_ptr->num = size;//size
	for(i=0;i<size;i++) {
		report_ptr->data[i] = (u8)i;//data
	}
	cs = 0;
	for(i=2;i<sizeof(DEV_COM_UART_REPORT)+2;i++) {
		cs += report_buff[i];
	}
	report_buff[sizeof(DEV_COM_UART_REPORT)+2] = 0-cs;//check sum
	sendUdpDatagram(report_buff,sizeof(DEV_COM_UART_REPORT)+3);//send udp datagram
}

/** apply new config */
void updateConfig(u8* ptr) {
	COM_DEV_CONFIG *msg = (COM_DEV_CONFIG*)ptr;
	SYS_CONFIG n_cfg;
	if(msg->head.len==(sizeof(COM_DEV_CONFIG)+1)) {
		mymemcpy((void*)n_cfg.ip_addr,(void*)msg->ip_addr,sizeof(SYS_CONFIG)-2);
		setConfig(n_cfg);
	}
}

/** handle protocol */
void handleProtocol(u8* ptr) {
	MESSAGE_HEAD *head = (MESSAGE_HEAD*)ptr;//get head
	if(head->code==0xA1) {
		//data
		sendUart(ptr);
	}
	else if(head->code==0xA2) {
		//reset
		resetUartFsmc();
	}
}

/** unpacking */
void unpacking(u8* ptr,u32 size) {
	u32 i=0;
	u8  ch,cs;
	while(i<size) {
		ch = ptr[i];
		i++;
		if(step==0) {
			//head 0
			if(ch == 0xaa) {
				step = 1;
			}
		}
		else if(step==1) {
			//head 1
			if(ch == 0xaa) {
				step = 2;
				index_buff = 0;
			}
			else {
				step = 0;
			}
		}
		else if(step==2) {
			//length 1
			udp_len = ch;
			udp_buff[index_buff] = ch;
			index_buff++;
			step = 3;
		}
		else if(step==3) {
			//length 2
			udp_len += ((u16)ch)<<8;
			udp_buff[index_buff] = ch;
			index_buff++;
			if(udp_len<1024) {
				step = 4;
			}
			else {
				step = 0;
			}
		}
		else if(step==4) {
			//content
			if(index_buff<udp_len-1) {
				udp_buff[index_buff] = ch;
				index_buff++;
				if(index_buff==udp_len-1) {
					step = 5;
				}
			}
			else {
				step = 0;
			}
		}
		else if(step==5) {
			//cs
			udp_buff[index_buff] = ch;
			cs = 0;
			for(index_buff=0;index_buff<udp_len;index_buff++) {
				cs += udp_buff[index_buff];
			}
			if(cs==0) {
				handleProtocol(udp_buff);
			}
			step = 0;
		}
	}
}

/** handler of received data */
void onRecvData(u8* ptr,u32 size) {
	unpacking(ptr,size);
}
