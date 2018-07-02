/*
 * Reed_Switch.c
 *
 *  Created on: Jun 30, 2018
 *      Author: Poornachander
 */

#include "stm32f0xx.h"

#define MICRO_POWER_ON	GPIO_Pin_8

//Micro-Power-On pin PA8

extern void init_Reed_Switch() {
		GPIO_InitTypeDef GPIO_InitStructure;

		//Enable the A port
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

		//Configure pin PA8
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

		GPIO_Init(GPIOA, &GPIO_InitStructure);

		GPIOA->ODR &= ~(GPIO_Pin_8);

}
