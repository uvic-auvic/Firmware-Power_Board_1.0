/*
 * LED.c
 *
 *  Created on: May 17, 2018
 *      Author: e_m20
 */

#include "stm32f0xx_rcc.h"
#include "stm32f0xx.h"

extern void init_LED() {
		GPIO_InitTypeDef GPIO_InitStructure;

		//Enable the C port
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

		//For now use the GPIO_Init function, to save time
		//Be careful because not erasing any of the past settings

		//Configure pin PC8(blue LED) and pin PC9(green LED) as output
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

		GPIO_Init(GPIOC, &GPIO_InitStructure);

}
