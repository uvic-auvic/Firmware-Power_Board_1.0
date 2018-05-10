/*
 * FSM.c
 *
 *  Created on: Feb 3, 2017
 *      Author: asus-andy
 */
#include "FSM.h"
#include "Buffer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ADC.h"

#define CHAR_TO_INT (48)

extern void FSM(void *dummy){
	//initialize the UART
	UART_init();

	inputBuffer.size = 0;

	//temporary storage to return from buffer
	char commandString[MAX_BUFFER_SIZE] = "";
	int tempVar;

	while(1){
		//it's important that this is while, if the task is accidentally awaken it
		//can't execute without having at least one item the input puffer
		while(inputBuffer.size == 0){
			//sleeps the task into it is notified to continue
			ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
		}
		//Write a statement here to do a string comparison on commands
		Buffer_pop(&inputBuffer, commandString);
		char argument = commandString[3];
		commandString[3] = '\0';

		char tempOutputString[MAX_BUFFER_SIZE] = "";

		//WTH
		if(strcmp(commandString, "WTH") == 0){
			char bufs[5];
			uint16_t pressure = Get_ADC_Channel(ADC_Water_Sensor);
			itoa(pressure, bufs, 10);
			UART_push_out_len(bufs, 5);
			UART_push_out_len("\r\n", 2);
		}

		//WTR
		else if(strcmp(commandString, "WTR") == 0){
			uint16_t pressure = Get_ADC_Channel(ADC_Water_Sensor);
			UART_push_out_len((char *)&pressure, 2);
			UART_push_out_len("\r\n", 2);
		}

		//RID
		else if(strcmp(commandString, "RID") == 0){
			UART_push_out("Power_");
			UART_push_out("Board");
			UART_push_out("\r\n");
		}
		//catch all error
		else{
			UART_push_out("error: ");
			UART_push_out(commandString);
			UART_push_out("\r\n");
		}
	}
}

void FSM_Init(){
	Buffer_init(&inputBuffer);

	// Create the FSM task
    xTaskCreate(FSM,
		(const signed char *)"FSM",
		configMINIMAL_STACK_SIZE,
		NULL,                 // pvParameters
		tskIDLE_PRIORITY + 1, // uxPriority
		NULL              ); // pvCreatedTask */
}
