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

#include "FreeRTOSConfig.h"

#include "FreeRTOS.h"
#include "task.h"

#include "BlinkLED.h"

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compilfer diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


/*The task turns on pin 8 for 2 seconds, then shuts off.
 * Afterwards runs a continuous loop with pin 9 flashing with 2 Hz.
 *
 * */
void blinkyTask(void *dummy){
	GPIOC->ODR |= GPIO_ODR_8;
	vTaskDelay(2000);
	GPIOC->ODR &= ~(GPIO_ODR_8);
	while(1){
		GPIOC->ODR ^= GPIO_ODR_9;
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
}

int
main(int argc, char* argv[])
{
	//called from blinkLED.h file to setup the pins
	initBlinkyLED();
	vGeneralTaskInit();
	/* Start the kernel.  From here on, only tasks and interrupts will run. */
	vTaskStartScheduler();
	// Should never get here
	while (1);
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
