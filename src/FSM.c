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
#include "sensors.h"

#define CHAR_TO_INT (48)

extern void FSM(void *dummy){
	//initialize the UART
	UART_init();

	inputBuffer.size = 0;

	//Stores the current command
	char commandString[MAX_BUFFER_SIZE] = "";

	while(1){
		//it's important that this is while, if the task is accidentally awaken it
		//can't execute without having at least one item the input puffer
		while(inputBuffer.size == 0){
			//sleeps the task into it is notified to continue
			ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
		}
		//Write a statement here to do a string comparison on commands
		Buffer_pop(&inputBuffer, commandString);

		//RID
		if(strcmp(commandString, "RID") == 0){
			UART_push_out("Power_");
			UART_push_out("Board");
			UART_push_out("\r\n");
		}

		//CRx command
		else if(strncmp(commandString, "CR", 2) == 0) {
			uint16_t current = 0; //Current in mA

			if(commandString[2] == '1') {
				//Battery 1
				current = Get_Battery_Current(Left_Battery);

			} else if(commandString[2] == '2') {
				//Battery 2
				current = Get_Battery_Current(Right_Battery);

			} else if(commandString[2] == 'M') {
				//Total Motor Current
				//Uses I2C chip, Not implemented yet
				current = Get_Motors_Current();

			} else if(commandString[2] == 'S') {
				//Total System current
				//Uses I2C chip, Not implemented yet
				current = Get_System_Current();

			}

			UART_push_out_len((char *)&current, 2);
			UART_push_out("\r\n");
		}

		//VTx command
		else if (strncmp(commandString, "VT", 2) == 0) {
				uint16_t voltage = 0; //Voltage in mV

				if (commandString[2] == '1') {
					voltage = Get_Battery_Voltage(Left_Battery);
				} else if (commandString[2] == '2') {
					voltage = Get_Battery_Voltage(Right_Battery);
				}

				UART_push_out_len((char *)&voltage, 1);
				UART_push_out("\r\n");
		}

		//BPx command
		else if (strncmp(commandString, "BP", 2) == 0) {
			if (commandString[2] == '1') {
				//enable battery bridge
			} else if (commandString[2] == '0') {
				//disable battery bridge
			}
		}

		//RBxyyy command
		else if (strncmp(commandString, "RB", 2) == 0) {

		}

		//PxEx command
		else if (commandString[0] == 'P' && commandString[2] == 'E') {

		}

		//TMP
		else if (strcmp(commandString, "TMP") == 0) {

		}

		//HUM command
		else if (strcmp(commandString, "HUM") == 0) {

		}

		//WTH
		else if(strcmp(commandString, "WTH") == 0){
			char bufs[5];
			uint16_t pressure = Get_Water_Sensor_Value();
			itoa(pressure, bufs, 10);
			UART_push_out_len(bufs, 5);
			UART_push_out_len("\r\n", 2);
		}

		//WTR
		else if(strcmp(commandString, "WTR") == 0){
			uint16_t pressure = Get_Water_Sensor_Value();
			UART_push_out_len((char *)&pressure, 2);
			UART_push_out_len("\r\n", 2);
		}

		//PIN command
		else if (strcmp(commandString, "PIN") == 0) {

		}

		//PEX command
		else if (strcmp(commandString, "PEX") == 0) {

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
