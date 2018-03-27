/*
 * timer.c
 *
 *  Created on: 2016年12月12日
 *      Author: hzhou
 */

#include "timer.h"
#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"
#include "msg_code_user.h"
#include "config.h"

u32 timer_cnt;
u32 cycle;
extern u32 lwip_localtime;

/** initialize timer */
void initTimer(void) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	SYS_CONFIG *cfg = getConfig();//get configuration

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///enable TIM3 clock

	//timing: interrupt every 5ms
	TIM_TimeBaseInitStructure.TIM_Period = 50-1; 	//period 50 --> 5ms
	TIM_TimeBaseInitStructure.TIM_Prescaler=8400-1;  //pre-scaler 8400 --> 10kHz
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //up mode
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;

	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//initialize

	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //allow interrupt
	TIM_Cmd(TIM3,ENABLE); //enable TIM3

	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //timer3 interrupt
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //priority 1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //sub priority 3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	timer_cnt = 0;
	
	//prepare cycle
	if(cfg->query_cycle_s>0) {
		cycle = 20*cfg->query_cycle_s;
	}
	else {
		cycle = 20;
	}
}

void timerProcess(void) {
	//LWIP time
	lwip_localtime += 5;
	
	//control work
	if((timer_cnt%cycle)==0) {
		//0.1s
		publishMsg(MSG_WORKTICK,(int)((timer_cnt/cycle)%10));
	}

	//increase count
	timer_cnt++;
	if(timer_cnt>=(cycle*10)) {
		timer_cnt = 0;
	}
}

/** handler of timer 3 interrupt */
void TIM3_IRQHandler(void) {
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) {
		timerProcess();
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
}
