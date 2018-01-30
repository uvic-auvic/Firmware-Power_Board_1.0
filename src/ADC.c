#include "stm32f0xx.h"
#include "stm32f0xx_adc.h"

#include "ADC.h"

extern uint16_t ADC_Buffer[BUFFER_SIZE];

static void DeInitBuffer(uint16_t *Buff){
	for(uint32_t i = 0; i<BUFFER_SIZE;i++){
			Buff[i] = 0;
		}
}


//GPIO
static void initADCPins_GPIO (){
	//Enable peripheral clock for GPIOA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6| GPIO_Pin_7);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

}

//ADC1 ON as cont. checking
static void initADCPins_ADC (){
	//Enable Peripheral clock for ADC1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	//ADC1 Structure
	ADC_InitTypeDef ADC_InitStructure;
	ADC_StructInit(&ADC_InitStructure);

	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;

	ADC_InitStructure.ADC_ExternalTrigConvEdge = 0;
	ADC_InitStructure.ADC_ExternalTrigConv = 0;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;

	//Load Structure into control and status register
	ADC_Init(ADC1, &ADC_InitStructure);

	//Config Channel
	ADC_ChannelConfig(ADC1, ADC_Channel_1, ADC_SampleTime_1_5Cycles);
	ADC_ChannelConfig(ADC1, ADC_Channel_2, ADC_SampleTime_1_5Cycles);
	ADC_ChannelConfig(ADC1, ADC_Channel_3, ADC_SampleTime_1_5Cycles);
	ADC_ChannelConfig(ADC1, ADC_Channel_4, ADC_SampleTime_1_5Cycles);

	//Enable peripheral
	ADC_Cmd(ADC1, ENABLE);

	//Enable interrupt
	ADC_ITConfig(ADC1, ADC_IT_ADRDY, ENABLE);

	uint32_t value = ADC_GetCalibrationFactor(ADC1);
}

static void initADCPins_DMA(){
	//Enable Peripheral Clock for DMA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	//Enable DMA for ADC1
	ADC_DMACmd(ADC1, ENABLE);
	ADC_DMARequestModeConfig(ADC1, ADC_CFGR1_DMACFG);

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
	DMA_InitStructure.DMA_BufferSize = BUFFER_SIZE;
	DMA_InitStructure.DMA_PeripheralBaseAddr = &(ADC1->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = ADC_Buffer;

	//Default Init Channel 1
	DMA_DeInit(DMA1_Channel1);
	//Load val's into Channel
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	//Enable DMA Interrupt
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	//Enable Channel 1
	DMA1_Channel1->CCR |= DMA_CCR_EN;
}

extern uint16_t Get_ADC_Channel(enum ADC_Channels channel){
	return ADC_Buffer[channel];
}

extern void initADCPins(){
	//init the ADC buffer;
	DeInitBuffer(ADC_Buffer);
	//turns PA0->PA7 ON
	initADCPins_GPIO();
	//turns the ADC ON
	initADCPins_ADC();

	initADCPins_DMA();
	ADC_StartOfConversion(ADC1);
}

