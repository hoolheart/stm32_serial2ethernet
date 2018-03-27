#ifndef __UART_FSMC_H__
#define __UART_FSMC_H__
#include "stm32f4xx.h"

#define UART_FSMC_NUM 16/**< channel number of fsmc-driven uart */

/** initialize uart from fsmc interface */
void initUartFsmc(void);
/** reset uart */
void resetUartFsmc(void);
/** send data to fsmc-driven uart */
u8 sendDataUartFsmc(u8 chl,u8 *ptr,u8 size);
/** size of pending recerved data of fsmc-driven uart */
u8 pendingNumUartFsmc(u8 chl);
/** receive data from fsmc-driven uart */
u8 receiveDataUartFsmc(u8 chl,u8 *ptr,u8 max);

#endif//__UART_FSMC_H__
