//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
//#include "diag/Trace.h"

#include "stm32f0xx_misc.h"
#include "stm32f0xx_tim.h"
#include <stm32f0xx_rcc.h>
#include "stm32f0xx_exti.h"
#include "stm32f0xx_syscfg.h"
#include "stm32f0xx.h"
#include "stm32f0xx_adc.h"

#include "FreeRTOSConfig.h"

#include "FreeRTOS.h"
#include "task.h"

#include "ADC.h"

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

uint16_t Values[BUFFER_SIZE];

void blinkyTask(void *dummy){
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	GPIOC->MODER |= 0x50000;
	GPIOC->ODR |= 0x100;
	vTaskDelay(2000);
	GPIOC->ODR &= ~(0x100);
	while(1){
		GPIOC->ODR ^= 0x200;
		vTaskDelay(500);
	}
}

void ADCTask(){
	ADC1->CR |= 0x4;//ADSTART: ADC Start Conversion Command
	uint16_t i = 0;
	while(1){//since the OVR bit is set to 1.the DMA transfers do not work
		uint16_t value2 = ADC_GetConversionValue(ADC1);
		uint16_t value = Get_ADC_Channel(ADC_Water_Sensor);
		Values[i] = value;
		uint16_t InputValue = Values[i];
		i++;
		i %= 8;
		DMA1->IFCR |= 0x7;
			/* CGIF1: Channel 1 Global Interrupt Clear
			 * CTCIF1: Channel 1 Transfer Complete Clear
			 * CHTIF1: Channel 1 Half Transfer Clear
			 */
		ADC1->ISR &= ~0x1E; //THEY DONT CLEAR
			/* OVR
			 * EOS
			 * EOC
			 * EOSMP
			 */
	}
	//Get_ADC_Channel only returns 0. but ADC_GetConversionValue(ADC1) returns values.
	//DMA is configured correctly. so the problem could be how the values are getting retrieved.
	//the ADC_Buffer has all values set to 0 by line 59
}

void vGeneralTaskInit(void){
   xTaskCreate(blinkyTask,
		(const signed char *)"blinkyTask",
		configMINIMAL_STACK_SIZE,
		NULL,                 // pvParameters
		tskIDLE_PRIORITY + 1, // uxPriority
		NULL              ); // pvCreatedTask */

   xTaskCreate(ADCTask,
    	(const signed char *)"ADCTask",
    	configMINIMAL_STACK_SIZE,
   		NULL,                 // pvParameters
    	tskIDLE_PRIORITY + 1, // uxPriority
    	NULL              ); // pvCreatedTask
}

int
main(int argc, char* argv[])
{
	//GPIO + ADC1 + DMA
	init();

	vGeneralTaskInit();
	/* Start the kernel.  From here on, only tasks and interrupts will run. */
	vTaskStartScheduler();

	// Should never get here
	while (1);
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
