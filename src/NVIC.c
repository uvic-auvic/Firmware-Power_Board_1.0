/*
 * NVIC.c
 *
 *  Created on: Jan 26, 2018
 *      Author: amann
 */
#include "stm32f0xx.h"
#include "stm32f0xx_misc.h"

//Enable interrupt for DMA Channel 1
extern void initNVIC (){
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
}
