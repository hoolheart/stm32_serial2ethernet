#ifndef __MSG_INTERFACE_H__
#define __MSG_INTERFACE_H__
#include "stm32f4xx.h"

/**
 * This file builds up an interface of message engine
 */

/** message code start for hardware parts */
#define MSG_START_HW   0
/** message code start for user parts */
#define MSG_START_USER 0x1000

/** initialize interface */
void initMsgInterface(void);
/** publish a message */
int publishMsg(int32_t msg,int32_t para);
/** get pending message number */
u32 getPendingMsgNum(void);
/** get next pending message */
int32_t getNextMsg(int32_t *msg,int32_t *para);

#endif //__MSG_INTERFACE_H__
