#ifndef __UDP_INTERFACE_H__
#define __UDP_INTERFACE_H__
#include "stm32f4xx.h"

typedef void (*UDP_HANDLER)(u8* ptr,u32 size);

/** parepare udp connection */
void prepareUDPConnection(UDP_HANDLER handler);
/** send udp datagram */
void sendUdpDatagram(u8* ptr,u32 size);
/** handle received udp */
void onUdpReceived();

#endif //__UDP_INTERFACE_H__
