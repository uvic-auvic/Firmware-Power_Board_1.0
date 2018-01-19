#include "stm32f0xx.h"
#include "stm32f0xx_adc.h"

/*			Questions
 * 			What does it mean to right align 12-bit data in the data register
 * 			Does the ADC need Direct Memory Access or is that just for UART/USART
 *
 */

//GPIO
static void initADCPins_GPIO (){
	//Enable peripheral clock for GPIOA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	//Object for GPIO pins
	GPIO_InitTypeDef GPIO_InitStructure;
	//Initialize the Object
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin |= GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	//Set GPIO as ANALOG
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	//Initialize the GPIO with the object settings
	GPIO_Init(GPIOA, &GPIO_InitStructure);

}
//ADC1 ON as continuous checking
static void initADCPins_ADC (){
	//Enable Peripheral clock for ADC1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	//ADC1 Object Initialization
	ADC_InitTypeDef ADC_InitStructure;
	/*
	 * the Hydrophone has another method to enable channels
	 * ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	 *
	 * ill keep the ADC_ChannelConfig but could be a possible problem
	 */
	//Continuous conversion mode
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	//No external Triggering
	ADC_InitStructure.ADC_ExternalTrigConvEdge = 0;
	ADC_InitStructure.ADC_ExternalTrigConv = 0;
	//right 12-bit Data alignment in ADC data reg.
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	//No ADC conversions

	//Load Object values to the control and status register
	ADC_Init(ADC1, &ADC_InitStructure);

	//Enable channels 1-4 of ADC1 to 1.5 cycles
	ADC_ChannelConfig(ADC1, ADC_Channel_1, ADC_SampleTime_1_5Cycles);
	ADC_ChannelConfig(ADC1, ADC_Channel_2, ADC_SampleTime_1_5Cycles);
	ADC_ChannelConfig(ADC1, ADC_Channel_3, ADC_SampleTime_1_5Cycles);
	ADC_ChannelConfig(ADC1, ADC_Channel_4, ADC_SampleTime_1_5Cycles);

	//Enable ADC ready interrupt
	ADC_ITConfig(ADC1, ADC_IT_ADRDY, ENABLE);
	//enables peripheral with ADEN = 1
	ADC_Cmd(ADC1, ENABLE);

}

//Initialize
extern void initADC1Pins(){
	//turns PA1->PA4 ON
	initADCPins_GPIO();
	//turns the ADC ON
	initADCPins_ADC();
}
