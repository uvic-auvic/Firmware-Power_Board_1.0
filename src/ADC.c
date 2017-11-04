#include "stm32f0xx.h"
#include "stm32f0xx_adc.h"

//Turns C-pins PA1->PA4 ON
static void initADCPins_GPIO (){
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);//Clock on for C pins
	GPIOA->MODER |= GPIO_MODER_MODER1 | GPIO_MODER_MODER2 |
					GPIO_MODER_MODER3| GPIO_MODER_MODER4; // PA1->PA4 on as analog
}
//Turns ADCx ON as continuous checking
static void initADCPins_ADC (ADC_TypeDef* ADCx){//add ADC_IER,
	//enables peripheral x by ADEN = 1
	ADC_Cmd(ADCx, ENABLE);
	//set ADCx to default values
	ADC_StructInit(ADCx);
	//Enable channels 1-4 of ADCX to 1.5 cycles
	ADC_ChannelConfig(ADCx, ADC_Channel_1, ADC_SampleTime_1_5Cycles);
	ADC_ChannelConfig(ADCx, ADC_Channel_2, ADC_SampleTime_1_5Cycles);
	ADC_ChannelConfig(ADCx, ADC_Channel_3, ADC_SampleTime_1_5Cycles);
	ADC_ChannelConfig(ADCx, ADC_Channel_4, ADC_SampleTime_1_5Cycles);
	//continuous checking on
	ADC_ContinuousModeCmd(ADCx, ENABLE);
	//Enable ADC ready interrupt
	ADC_ITConfig(ADCx, ADC_IT_ADRDY, ENABLE);

}

//Initialize
void initADCPins(ADC_TypeDef* ADCx){
	initADCPins_GPIO();//turns PA1->PA4 ON
	initADCPins_ADC(ADCx);//turns the ADC ON
}
