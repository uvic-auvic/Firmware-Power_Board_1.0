/*
 * PBInitialize.c
 *
 *  Created on: Feb 11, 2019
 *      Author: Gabriel
 */

/* RTOS Includes */
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

/* Startup */
#include "LED.h"
#include "Reed_Switch.h"
#include "ADC.h"
#include "I2C.h"
#include "sensors.h"
#include "high_side_drives.h"
#include "FSM.h"
#include "PBInitialize.h"

#define STARTUP_WAIT	2000000

/*
 * Purpose:		Initializes PowerBoard, starts all tasks
 * param@[in] 	void
 * Returns:		void
 */
void PB_Initialize()
{

	//LED + GPIO + ADC1 + DMA
	for(uint32_t i = 0; i < 2000000; i++);

	init_LED();
	init_Reed_Switch();
	initADC();
	I2C_init(); // Find out if can be broken into tasks
	init_Sensors();
	init_HSDs();
	FSM_Init(); //TODO: All of theses should be a task, with startup and while loops.

	blinkyTaskInit();

	/* Start the kernel.  From here on, only tasks and interrupts will run. */
	vTaskStartScheduler(); //task.c
}

void blinkyTaskInit(void)  //TODO: Create All startup tasks here
{
   xTaskCreate( blinkyTask, (const char *)"blinkyTask", configMINIMAL_STACK_SIZE,
		   NULL /*pvParameters*/, tskIDLE_PRIORITY + 1/*uxPriority*/, NULL /* pvCreatedTask */ );

}

void blinkyTask(void *dummy){
	GPIOC->ODR |= GPIO_Pin_8; // To turn on the green LED

	uint8_t counter = 0;
	while (counter < 20)
	{
		GPIOC->ODR ^= GPIO_Pin_9;
		vTaskDelay(500);     // speed of the delay
		counter++;
	}

	GPIOC->ODR &= ~(GPIO_Pin_8);   // TO turn off the green LED

	while(1)
	{       // while loop for blinking
			GPIOC->ODR ^= GPIO_Pin_9;
			vTaskDelay(500);     // speed of the delay
	}
}
