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

void ADCTask(ADC_TypeDef* ADCx){
	/*ADSTART = ON/1 (declare once b/c conversions are continuous)
	 * The conversion will start immediately*/
	ADC_StartOfConversion(ADCx);
	//CONT=1: start sequence, get DR data and Disable EOC flag
	while(1){
		ADC_GetConversionValue(ADCx);
		ADC_ITConfig(ADCx, ADC_IT_EOC, DISABLE);
	}
	//after sequence, Disable EOSEQ flag and go into wait mode.
	ADC_ITConfig(ADCx, ADC_IT_EOSEQ, DISABLE);
	ADC_WaitModeCmd(ADCx, ENABLE);

}

void blinkyTask(void *dummy){
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	GPIOC->MODER |= 0x50000;
	GPIOC->ODR |= 0x100; // 0001 0000 0000
	vTaskDelay(2000);
	GPIOC->ODR &= ~(0x100); //clears pin 8 (0b0000 0000 0000)
	while(1){
		GPIOC->ODR ^= 0x200;

		/*0 1 2 3 4 5 6 7 8 9 A  B  C  D  E  F
		0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
		0b1111 -> 15 -> 0xF

		0x3B5FA0 -> (# of bytes) = (number of digits) / 2*/

		vTaskDelay(500);
	}
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
			ADC1,                 // pvParameters
    		tskIDLE_PRIORITY + 2, // uxPriority
    		NULL              ); // pvCreatedTask */
}

int
main(int argc, char* argv[])
{
	//initialize ADC1 and turn on
	initADCPins(ADC1);
	vGeneralTaskInit();
	/* Start the kernel.  From here on, only tasks and interrupts will run. */
	vTaskStartScheduler();

	// Should never get here
	while (1);
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
