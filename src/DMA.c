/*
 * DMA.c
 *
 *  Created on: Jan 22, 2018
 *      Author: amann
 */

#include "stm32f0xx.h"
#include "stm32f0xx_dma.h"
#include "ADC.h"

void initDMA(){
	//Enable Periph Clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	//Create Structure
	DMA_InitTypeDef DMA_InitStructure;
	DMA_StructInit(&DMA_InitStructure);

	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 0x10;//16-bit
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint16_t)ADC_Channel;

	//Default Init Channel 1
	DMA_DeInit(DMA1_Channel1);
	//Load val's into Register
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	//Enable DMA Interrupt
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);


}
