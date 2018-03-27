/*
 * debug_led.c
 *
 *  Created on: SEP 9, 2016
 *      Author: hzhou
 */
#include "debug_led.h"
#include "stm32f4xx_conf.h"

uint16_t LEDPins[5] = {GPIO_Pin_0, GPIO_Pin_3, GPIO_Pin_5, GPIO_Pin_4, GPIO_Pin_9};

#define LED_ADDR 5

/** initialize debug LED interface */
void initDebugLED()
{
	// prepare configuration
	GPIO_InitTypeDef GPIO_InitStructure;

	//enable clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

	//initialize GPIO
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;//alternative mode
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz speed
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//PP output
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;//pull up
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	//default to close all
	closeAllLED();
}

/** open LED */
void openLED(unsigned char index)
{
	u8 tmp;
	if(index<LED_ADDR) {//check index
		GPIO_ResetBits(GPIOA,LEDPins[index]);//set Pin low
	}
}

/** close LED */
void closeLED(unsigned char index)
{
	u8 tmp;
	if(index<LED_ADDR) {//check index
		GPIO_SetBits(GPIOA,LEDPins[index]);//set Pin high
	}
}

/** open all LEDs */
void openAllLED()
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_9);//set all LED pins low
}

/** close all LEDs */
void closeAllLED()
{
	GPIO_SetBits(GPIOA,GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_9);//set all LED pins high
}
