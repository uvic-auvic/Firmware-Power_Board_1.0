#include "stm32f0xx.h"
#include "stm32f0xx_adc.h"

extern uint16_t ADC_Channel[4];

//GPIO
static void initADCPins_GPIO (){
	//Enable peripheral clock for GPIOA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

}

//ADC1 ON as single checking
static void initADCPins_ADC (){
	//Enable Peripheral clock for ADC1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	//ADC1 Structure
	ADC_InitTypeDef ADC_InitStructure;
	ADC_StructInit(&ADC_InitStructure);

	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_DiscModeCmd(ADC1, ENABLE);
	//if continous mode is disabled, is disc mode automatically enabled?

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
	//Calibrate <<This doesnt seem right>>
	uint32_t value = ADC_GetCalibrationFactor(ADC1);
}
static void ADC_DMAConfig(){
	//Enable DMA for ADC1
	ADC_DMACmd(ADC1, ENABLE);
	ADC_DMARequestModeConfig(ADC1, ADC_CFGR1_DMACFG);
}

//Initialize
extern void initADCPins(){
	//turns PA1->PA4 ON
	initADCPins_GPIO();
	//turns the ADC ON
	initADCPins_ADC();
	//Configure DMA for ADC1
	ADC_DMAConfig();
	for(uint32_t i = 0; i<4;i++){
		ADC_Channel[i] = 0;
	}
}

