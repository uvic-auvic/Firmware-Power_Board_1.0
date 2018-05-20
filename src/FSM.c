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
#include <stdlib.h>
#include <stdio.h>

#define CHAR_TO_INT (48)

extern void FSM(void *dummy){
	//initialize the UART
	UART_init();

	UARTinputBuffer.size = 0;

	//Stores the current command
	char commandString[MAX_BUFFER_SIZE] = "";

	while(1){
		//it's important that this is while, if the task is accidentally awaken it
		//can't execute without having at least one item the input puffer
		while(UARTinputBuffer.size == 0){
			//sleeps the task into it is notified to continue
			ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
		}

		memset(commandString, 0, MAX_BUFFER_SIZE);
		Buffer_pop(&UARTinputBuffer, commandString);

		//RID
		if(strcmp(commandString, "RID") == 0){
			UART_push_out("Power_");
			UART_push_out("Board");
			UART_push_out("\r\n");
		}

		//Hardware issue with op-amp causes values below 45mV and values above 2.75V to return incorrect ADC values.
		//This shouldn't be a problem with normal operation. Only with very low currents.
		//CRx & CRA command
		else if(strncmp(commandString, "CR", 2) == 0) {
			uint32_t current = 0; //Current in mA

			if(commandString[2] == '1') {
				//Battery 1
				current = Get_Battery_Current(Left_Battery);
				UART_push_out_len((char *)&current, 3);

			} else if(commandString[2] == '2') {
				//Battery 2
				current = Get_Battery_Current(Right_Battery);
				UART_push_out_len((char *)&current, 3);

			} else if(commandString[2] == 'M') {
				//Total Motor Current
				//Uses I2C chip, Not implemented yet
				current = Get_Motors_Current();
				UART_push_out_len((char *)&current, 3);

			} else if(commandString[2] == 'S') {
				//Total System current
				//Uses I2C chip, Not implemented yet
				current = Get_System_Current();
				UART_push_out_len((char *)&current, 3);

			} else if(commandString[2] == 'A') {
				uint32_t cr1 = Get_Battery_Current(Left_Battery);
				uint32_t cr2 = Get_Battery_Current(Right_Battery);
				uint32_t crm = Get_Motors_Current();
				uint32_t crs = Get_System_Current();
				UART_push_out_len((char *)&cr1,3);
				UART_push_out_len((char *)&cr2,3);
				UART_push_out_len((char *)&crm,3);
				UART_push_out_len((char *)&crs,3);
			}

			UART_push_out("\r\n");

		}

		//VTx & VTA command
		else if (strncmp(commandString, "VT", 2) == 0) {
				uint16_t voltage = 0; //Voltage in mV

				if (commandString[2] == '1') {
					voltage = Get_Battery_Voltage(Left_Battery);
					UART_push_out_len((char *)&voltage, 2);
				} else if (commandString[2] == '2') {
					voltage = Get_Battery_Voltage(Right_Battery);
					UART_push_out_len((char *)&voltage, 2);
				} else if (commandString[2] == 'A') {
					uint16_t vt1 = Get_Battery_Voltage(Left_Battery);
					uint16_t vt2 = Get_Battery_Voltage(Right_Battery);
					UART_push_out_len((char *)&vt1,2);
					UART_push_out_len((char *)&vt2,2);
				}

				UART_push_out("\r\n");
		}
		//BPx command
		else if (strncmp(commandString, "BP", 2) == 0) {
			if (commandString[2] == '1') {
				//enable battery bridge
			} else if (commandString[2] == '0') {
				//disable battery bridge
			}

			//Returns ACK for now so software can begin testing
			UART_push_out("ACK\r\n");
		}

		//RBxyyy command, Reboot Subsystem
		else if (strncmp(commandString, "RB", 2) == 0) {

			//Returns ACK for now so software can begin testing
			UART_push_out("ACK\r\n");
		}

		//PxEx command, Power Motor/System Enable/Disable
		else if (commandString[0] == 'P' && commandString[2] == 'E') {

			//Returns ACK for now so software can begin testing
			UART_push_out("ACK\r\n");
		}

		//TMP command, Temperature
		else if (strcmp(commandString, "TMP") == 0) {
			uint16_t temperature = Get_Temperature();

			UART_push_out_len((char *)&temperature, 2);
			UART_push_out("\r\n");
		}

		//HUM command, Humidity
		else if (strcmp(commandString, "HUM") == 0) {
			uint16_t humidity = Get_Temperature();

			UART_push_out_len((char *)&humidity, 2);
			UART_push_out("\r\n");
		}

		//Hardware issue with op-amp causes values below 45mV and values above 2.75V to return incorrect ADC values.
		//WTH
		else if(strcmp(commandString, "WTH") == 0){
			char bufs[6] = "";
			uint16_t pressure = Get_Water_Sensor_Value();
			itoa(pressure, bufs, 10);
			UART_push_out_len(bufs, 5);
			UART_push_out_len("\r\n", 2);
		}

		//Hardware issue with op-amp causes values below 45mV and values above 2.75V to return incorrect ADC values.
		//WTR
		else if(strcmp(commandString, "WTR") == 0){
			uint16_t pressure = Get_Water_Sensor_Value();

			UART_push_out_len((char *)&pressure, 2);
			UART_push_out_len("\r\n", 2);
		}

		//PIN command
		else if (strcmp(commandString, "PIN") == 0) {
			uint16_t pressure = Get_Temperature();

			UART_push_out_len((char *)&pressure, 2);
			UART_push_out("\r\n");
		}

		//Hardware issue with op-amp causes values below 45mV and values above 2.75V to return incorrect ADC values.
		//PEX command
		else if (strcmp(commandString, "PEX") == 0) {
			uint16_t pressure = Get_External_Pressure();

			UART_push_out_len((char *)&pressure, 2);
			UART_push_out_len("\r\n", 2);
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
	Buffer_init(&UARTinputBuffer);

	// Create the FSM task
    xTaskCreate(FSM,
		(const signed char *)"FSM",
		configMINIMAL_STACK_SIZE,
		NULL,                 // pvParameters
		tskIDLE_PRIORITY + 1, // uxPriority
		NULL              ); // pvCreatedTask */
}
