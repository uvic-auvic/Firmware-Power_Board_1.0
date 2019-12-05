/*
 * reset_handler.c
 *
 *  Created on: Nov 28, 2019
 *      Author: Danielle
 */
#include "reset_handler.h"
#include "FreeRTOS.h"
#include "task.h"
#include "high_side_drives.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

char* current_reset_command = "";
TaskHandle_t reset_task;

void reset_handler(){


	//flags for each section
	bool power_off_motor = false;
	bool power_off_system = false;


	//Sets flags to reset specified sections
	if (current_reset_command[2] == 'M') {

			power_off_motor = true;

	} else if (current_reset_command[2] == 'S'){

			power_off_system = true;

	} else if (current_reset_command[2] == 'A'){

			power_off_motor = true;
			power_off_system = true;

	} else {

		UART_push_out("ERROR\r\n");

	}


	//converts last 3 chars of reset command (time input in seconds) to integer
	int time_input =
			100*(current_reset_command[3]-0)+
			10* (current_reset_command[4]-0)+
			1*  (current_reset_command[5]-0);




	//powers off selected sections
	if(power_off_motor && power_off_system){

		power_enable(motor_power, off);
		power_enable(system_power, off);

	} else if(power_off_motor){

		power_enable(motor_power, off);

	} else if(power_off_system){

		power_enable(system_power, off);

	}



	//delays this task before turning power back on;
	vTaskDelay(time_input*1000);



	//turns on sections that have been turned off and responds to Jetson
	if(power_off_motor && power_off_system){

		power_enable(motor_power, on);
		power_enable(system_power, on);
		UART_push_out("ACK\r\n");

	} else if(power_off_motor){

		power_enable(motor_power, on);
		UART_push_out("ACK\r\n");

	} else if(power_off_system){

		power_enable(system_power, on);
		UART_push_out("ACK\r\n");

	}



	//deletes the task until initialized again
	vTaskDelete( reset_task );

	return;
}

void reset_handlerInit(char* input_command){

	current_reset_command = input_command;

    xTaskCreate(reset_handler,
		(const signed char *)"reset_handler",
		configMINIMAL_STACK_SIZE,
		NULL,                 // pvParameters
		tskIDLE_PRIORITY + 1, // uxPriority
		&reset_task             ); // pvCreatedTask */

}
