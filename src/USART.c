#include <string.h>


#include "stm32f0xx.h"
#include "USART.h"
#include "Buffer.h"

/* Definitions */
#define MAX_LENGTH (8)


/* Global Variables */
CharBuffer_t RXBuffer;
Buffer_t Buffer1;

/* -------------------------------- Configuration ------------------------------------ */
/*
 *  @brief  Configures the USART1 pins on GPIO PA10 PA9
             - PA10 = Receive
           	 - PA9 = Transmit
  * @param  None
  * @retval None
*/
static void Configure_GPIO(){
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER |= GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1;/* AF mode */
	GPIOA->AFR[1] |= 0x110; /* Enable AF1 */
}
/*  @brief  Configures USART1
  * @param  None
  * @retval None
*/
static void Configure_USART(){
	/* Enable Peripheral */
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	/* Baudrate set at 9600 Kbps */
	USART1->BRR = 480000 / 96;

	/* Enable      Transmitter   Receiver       RXNE interrupt     USART */
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_UE;

	/* Polling idle frame Transmission w/o clock */
	while ((USART1->ISR & USART_ISR_TC) != USART_ISR_TC)
	{
	 /* add time out here for a robust application */
	}
	USART1->ICR |= USART_ICR_TCCF; /* Clear TC flag */
	USART1->CR1 |= USART_CR1_TCIE; /* Enable TC interrupt */

	/* Configure IT */
	/* (3) Set priority for USART1_IRQn */
	/* (4) Enable USART1_IRQn */
	NVIC_SetPriority(USART1_IRQn, 0); /* (3) */
	NVIC_EnableIRQ(USART1_IRQn); /* (4) */
}
extern void initUSART(){
	CharBuffer_init(&RXBuffer);
	Buffer_init(&Buffer1);
	Configure_GPIO();
	Configure_USART();
}

/* ---------------------------------- Function -------------------------------------- */
inline static void ReceiveChar(char charToReceive){
	/*  Check for the Null terminator and Carriage return */
	if((charToReceive == '\r') || (charToReceive == '\n')){
		CharBuffer_to_Buffer(&RXBuffer, &Buffer1);
	}
	/* Check if the index is at max length */
	else if(RXBuffer.size == MAX_LENGTH){
		CharBuffer_to_Buffer(&RXBuffer, &Buffer1);
	}
	else{
		CharBuffer_add(&RXBuffer, charToReceive);
	}
}

void USART1_IRQHandler(){
	/* Clear the TC Flag after sending out from ReceiveChar */
	if((USART1->ISR & USART_ISR_TC) == USART_ISR_TC){
		USART1->ICR |= USART_ICR_TCCF; /* clear TC flag */
	}
	/* Receive Char */
	else if((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE){
		ReceiveChar((uint8_t)(USART1->RDR));
	}
	/* Disable USART1_IRQn */
	else{
		NVIC_DisableIRQ(USART1_IRQn);
	}
}
