#include "stm32f0xx.h"
#include "ADC.h"

extern uint16_t ADC_Buffer[BUFFER_SIZE];

static void DeInitBuffer (uint16_t *Buff){
	for(uint32_t i = 0; i<BUFFER_SIZE;i++){
			Buff[i] = 0;
		}
}

static void ADC_Calibration (){
	ADC1->CR &= ~ADC_CR_ADEN;
	ADC1->CR |= ADC_CR_ADCAL;//WHY WONT YOU TURN TO 1
	while((ADC1->CR & ADC_CR_ADCAL) != 0){
		//TIME-OUT MANAGEMENT
	}
}

static void init_GPIO (){
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
//ADC1 Continuous Conversion Software Trigger
static void init_ADC (){
	RCC->APB2ENR |= RCC_APB2RSTR_ADC1RST;
	ADC_Calibration();
	ADC1->CR |= ADC_CR_ADEN;
	ADC1->CFGR1 |= ADC_CFGR1_CONT;
	ADC1->CHSELR |= ADC_CHSELR_CHSEL7	| ADC_CHSELR_CHSEL6
				 | ADC_CHSELR_CHSEL5 	| ADC_CHSELR_CHSEL4
				 | ADC_CHSELR_CHSEL3 	| ADC_CHSELR_CHSEL2
				 | ADC_CHSELR_CHSEL1 	| ADC_CHSELR_CHSEL0;
}

static void init_DMA (){
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	ADC1->CFGR1 |= ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG;
	DMA1_Channel1->CPAR = (uint32_t) (&(ADC1->DR));		//Peripheral Address
	DMA1_Channel1->CMAR = (uint32_t) ADC_Buffer;		//Memory Address
	DMA1_Channel1->CNDTR = BUFFER_SIZE;					//Memory Size
	DMA1_Channel1->CCR |= DMA_CCR_MINC		| DMA_CCR_MSIZE_0
					   | DMA_CCR_PSIZE_0 	| DMA_CCR_CIRC;
	DMA1_Channel1->CCR |= DMA_CCR_EN;
}

extern uint16_t Get_ADC_Channel (enum ADC_Channels channel){
	return ADC_Buffer[channel];
}

extern void initADC(){
	DeInitBuffer(ADC_Buffer);//Init the ADC buffer;
	init_GPIO();//turns PA0->PA7 ON
	init_ADC();//turns the ADC ON
	init_DMA();//turns DMA ON

	ADC1->CR |= ADC_CR_ADSTART;
}
