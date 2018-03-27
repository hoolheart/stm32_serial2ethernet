#ifndef __PROTOCOL_LAYER_H__
#define __PROTOCOL_LAYER_H__
#include "stm32f4xx.h"

/** initialize protocol layer */
void initProtocolLayer(void);
/** handler of received data */
void onRecvData(u8* ptr,u32 size);
/** check uart */
u8 checkUartFsmc(u8 chl);
/** test uart */
void testUart(void);

#endif //__PROTOCOL_LAYER_H__
