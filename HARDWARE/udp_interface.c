#include "udp_interface.h"
#include "stm32f4xx.h"
#include "lwip_comm.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "malloc.h"
#include "config.h"
#include <stdio.h>

UDP_HANDLER udp_handler=0;

#define UDP_RX_BUFSIZE 2048

struct udp_pcb *udppcb=0;//udp control block
u8 udp_recvbuf[UDP_RX_BUFSIZE];//buffer

/** callback */
void udp_demo_recv(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port) {
	u32 data_len = 0, i;
	u8 *payload;
	struct pbuf *q;
	if(p && udp_handler) {
		for(q=p;q!=NULL;q=q->next) {//check entire chain
			payload = (u8*)q->payload;
			if(q->len > (UDP_RX_BUFSIZE-data_len)) {
				//not enough buffer
				for(i=0;i<(UDP_RX_BUFSIZE-data_len);i++) {
					udp_recvbuf[i+data_len] = payload[i];
				}
				data_len = UDP_RX_BUFSIZE;//increase length
			}
			else {
				//copy all data
				for(i=0;i<q->len;i++) {
					udp_recvbuf[i+data_len] = payload[i];
				}
				data_len += q->len;//increase length
			}
			if(data_len>=UDP_RX_BUFSIZE) {
				break;
			}
			(*udp_handler)(udp_recvbuf,data_len);//call handler
		}
	}
	if(p) {
		pbuf_free(p);//free pbuf
	}
}

/** parepare udp connection */
void prepareUDPConnection(UDP_HANDLER handler) {
	struct ip_addr remote_ip;//remote IP
	SYS_CONFIG *cfg = getConfig();//get configuration
	
	//prepare control block
	udppcb = udp_new();
	if(udppcb) {
		IP4_ADDR(&remote_ip,cfg->remote_ip[0],cfg->remote_ip[1],cfg->remote_ip[2],cfg->remote_ip[3]);//setup remote ip
		if(udp_bind(udppcb,IP_ADDR_ANY,cfg->listen_port)==ERR_OK) {//bind
			if(udp_connect(udppcb,&remote_ip,cfg->remote_port)==ERR_OK) {//connect remote
				udp_handler = handler;
				udp_recv(udppcb,udp_demo_recv,NULL);//register callback function
			}
		}
	}
}

/** send udp datagram */
void sendUdpDatagram(u8* ptr,u32 size) {
	struct pbuf *buff;
	
	if(udppcb) {
		buff = pbuf_alloc(PBUF_TRANSPORT,size,PBUF_POOL); //perpare buffer
		if(buff) {
			buff->payload=(void*)ptr; 
			udp_send(udppcb,buff);//send datagram
			pbuf_free(buff);
		} 
	}
}
