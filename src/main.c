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

#include "LED.h"
#include "FSM.h"
#include "ADC.h"
#include "high_side_drives.h"
#include "I2C.h"
#include "sensors.h"

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

void blinkyTask(void *dummy){
	GPIOC->ODR |= GPIO_Pin_8; // To turn on the blue LED
	vTaskDelay(2000);         // To delay the code by 2s
	GPIOC->ODR &= ~(GPIO_Pin_8);   // TO turn off the blue LED
	GPIOC->ODR |= GPIO_Pin_9;	// To turn on the green LED

	while(1){                   // while loop for blinking
			GPIOC->ODR ^= GPIO_Pin_9;
			vTaskDelay(500);     // speed of the delay
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

int main(int argc, char* argv[]) {

	//LED + GPIO + ADC1 + DMA
	init_LED();
	initADC();
	I2C_init();
	init_Sensors();
	init_HSDs();
  FSM_Init();
  power_enable(motor_power, on);

	vGeneralTaskInit();
	/* Start the kernel.  From here on, only tasks and interrupts will run. */
	vTaskStartScheduler();

	// Should never get here
	while (1);
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
