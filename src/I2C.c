/*
 * I2C.c
 *
 *  Created on: May 15, 2018
 *      Author: Poornachander
 */

#include "string.h"
#include "stm32f0xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define I2C_NBYTES_BIT	16
#define I2C_SADD_BIT	1

//I2C Output buffer
#define I2C_OUTPUT_BUFFER_SIZE	8
uint8_t I2C_OutputBuffer[I2C_OUTPUT_BUFFER_SIZE];
uint8_t I2C_outputBufferIndex_Head = 0;

//I2C Input Buffer
#define I2C_INPUT_BUFFER_SIZE	8
uint8_t *I2C_inputBuffer;

//FreeRTOS current task handle
TaskHandle_t TaskToNotify = NULL;

//FreeRTOR mutex
SemaphoreHandle_t I2C_mutex;

extern uint16_t switch_endiness_uint16(uint16_t input) {
	uint8_t temp = (input & 0xFF00) >> 8;
	input = (input & 0x00FF) << 8;
	input |= temp;
	return input;
}

extern uint32_t switch_endiness_uint32(uint32_t input, uint8_t numBytes) {

	uint32_t output = 0;

	for(uint8_t i = 0; i < numBytes; i++) {
		output = output << 8;
		output |= (input & 0xFF);
		input = input >> 8;
	}

	return output;
}

extern void I2C_init() {
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_1);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // This doesn't need a pull up
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	I2C_InitTypeDef I2C_InitStructure;

	/* I2C configuration */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	I2C1->CR1 = I2C_CR1_PE; //I don't think this is needed here

	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable; //
	I2C_InitStructure.I2C_DigitalFilter = 0x00; //
	I2C_InitStructure.I2C_OwnAddress1 = 0x00; //
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable; //
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; //
	I2C_InitStructure.I2C_Timing = 0x00B01A4B; //0x00201D2B

	//  /* (1) Timing register value is computed with the AN4235 xls file,
	//   fast Mode @400kHz with I2CCLK = 48MHz, rise time = 140ns, fall time = 40ns */

	/* Apply I2C configuration after enabling it */
	I2C_Init(I2C1, &I2C_InitStructure);

	I2C1->CR1 |= I2C_CR1_TCIE | I2C_CR1_TXIE | I2C_CR1_RXIE;

	/* sEE_I2C Peripheral Enable */
	I2C_Cmd(I2C1, ENABLE);

	/* Configure IT */
	/* (4) Set priority for I2C2_IRQn */
	/* (5) Enable I2C2_IRQn */
	NVIC_SetPriority(I2C1_IRQn, 6); /* (4) */
	NVIC_EnableIRQ(I2C1_IRQn); /* (5) */

	//Initialize I2C mutex
	I2C_mutex = xSemaphoreCreateMutex();
}

extern void I2C_read(uint8_t address, uint8_t numBytes, uint8_t *message) {
	TaskToNotify = xTaskGetCurrentTaskHandle();

	I2C_inputBuffer = message;

	I2C1->CR2 = (numBytes << I2C_NBYTES_BIT) | (address << I2C_SADD_BIT) | I2C_CR2_RD_WRN;
	I2C1->CR2 |= I2C_CR2_START;
}

extern void I2C_write(uint8_t address, uint8_t numBytes, uint8_t message[]) {
	TaskToNotify = xTaskGetCurrentTaskHandle();

	memcpy(I2C_OutputBuffer, message, numBytes);

	I2C1->CR2 = (numBytes << I2C_NBYTES_BIT) | (address << I2C_SADD_BIT);
	I2C1->CR2 &= ~I2C_CR2_RD_WRN;
	I2C1->CR2 |= I2C_CR2_START;

}

void I2C1_IRQHandler(void) {

	if((I2C1->ISR & I2C_ISR_RXNE) == I2C_ISR_RXNE) {
		*I2C_inputBuffer = I2C1->RXDR;
		I2C_inputBuffer++;

	} else if ((I2C1->ISR & I2C_ISR_TXIS) == I2C_ISR_TXIS) {
		I2C1->TXDR = I2C_OutputBuffer[I2C_outputBufferIndex_Head];
		I2C_outputBufferIndex_Head = (I2C_outputBufferIndex_Head + 1) % I2C_OUTPUT_BUFFER_SIZE;

	} else if((I2C1->ISR & I2C_ISR_TC) == I2C_ISR_TC) { //If all bytes have been transmitted
		I2C1->CR2 |=  I2C_CR2_STOP;
		I2C_outputBufferIndex_Head = 0;
		I2C_inputBuffer = 0;

		vTaskNotifyGiveFromISR(TaskToNotify, pdFALSE);

	} else if ((I2C1->ISR & I2C_ISR_NACKF) == I2C_ISR_NACKF) { //If a NACK is returned
		vTaskNotifyGiveFromISR(TaskToNotify, pdFALSE);

	}

}
