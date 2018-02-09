#include "stm32f0xx.h"
#include "ADC.h"

extern uint16_t ADC_Buffer[BUFFER_SIZE];

static void DeInitBuffer (uint16_t *Buff){
	for(uint32_t i = 0; i<BUFFER_SIZE;i++){
			Buff[i] = 0;
		}
}

static void ADC_Calibration (){//MAKE SURE ADEN = 0
	ADC1->CR &= ~(0x1);
	ADC1->CR |= 214748648;//doesnt set to 1
	while((ADC1->CR & 0x80000000) != 0){
		//ADD SOME STUFF FOR TIME-OUT MANAGEMENT
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
	RCC->APB2ENR |= 0x200;
	ADC_Calibration();
	ADC1->CR |= 0x1;//ADEN: ADC Enabled
	ADC1->CFGR1 |= 0x3000;
		/*Bits Enabled
		 * CONT: Continuous Conversion mode
		 * OVRMOD: Overrun management Mode =
		 * 			DR reg is overwritten with last conversion
		 * 			result when overrun is detected.
		 */
	ADC1->IER |= 0x1C;
		/*Bits Enabled
		 * EOCIE: End of Conversion Interrupt
		 * EOSEQIE: End of Conversion Sequence Interrupt
		 * OVRIE: Overrun Interrupt
		 */
	ADC1->CHSELR |= 0xFF;//Channel 1 -> 7 for conversion
}

static void init_DMA (uint16_t *Buff){
	RCC->AHBENR |= 0x1;//DMAEN: DMA Clock
	ADC1->CFGR1 |= 0x3;
		/* DMAEN: Direct Memory Access
		 * DMACFG: Direct Memory Access Configuration
		 */
	DMA1_Channel1->CPAR = (uint32_t) (&(ADC1->DR));//Peripheral Address [we only want 16-bits to be read
	DMA1_Channel1->CMAR = (uint32_t) Buff;//Memory Address
	DMA1_Channel1->CNDTR = BUFFER_SIZE;
	DMA1_Channel1->CCR = 0x20;//Circular Mode
	DMA1_Channel1->CCR |= 0x54A;
		/*Bits Enabled
		 * MSIZE: Memory Size is 16-bits
		 * PSIZE: Peripheral Size is 16-bit
		 * PINC: Peripheral Increment Mode
		 * TEIE: Transfer Error Interrupt
		 * TCIE: Transfer Complete Interrupt
		 */
	DMA1_Channel1->CCR |= 0x1;//EN: Channel Enable
}

extern uint16_t Get_ADC_Channel (enum ADC_Channels channel){
	return ADC_Buffer[channel];
}

extern void init (){
	DeInitBuffer(ADC_Buffer);//Init the ADC buffer;
	init_GPIO();//turns PA0->PA7 ON
	init_ADC();//turns the ADC ON
	init_DMA(ADC_Buffer);//turns DMA ON
}
