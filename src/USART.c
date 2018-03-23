#include <string.h>

#include "USART.h"
#include "stm32f0xx.h"
#include "Buffer.h"

/* Definitions */
#define MAX_LENGTH (8)

/* Private Variables */

static uint8_t bytes_to_send = 0;
char RXBuffer[MAX_LENGTH] = "";
int Curr_Pointer_Location = 0;

CharBuffer_t TempBuff;

/* Macros */
static void ResetRXBuffer(){
	for(int i = 0; i < MAX_LENGTH; i++){
		RXBuffer[i] = 0;
	}
	Curr_Pointer_Location = 0;
}

static void AppendToRXBuffer(char Data){
	RXBuffer[Curr_Pointer_Location] = Data;
	Curr_Pointer_Location++;
}
static uint8_t Ret_Curr_Val_RXBuffer(int indx){
	return RXBuffer[indx];
}
// ---------------------------------------------------------------------------------- */
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
	/* Store the handle of the calling task. */
	ResetRXBuffer();
	Configure_GPIO();
	Configure_USART();
}

// ---------------------------------------------------------------------------------- */
static void TransmitChar(uint8_t Data){
	/* Start USART transmission */
	USART1->TDR = Data; /* Clears TXE bit */
	while((USART1->ISR & USART_ISR_TC) == 0){
		/* After writing into the USART_TDR register, wait until TC=1. This indicates that the
		 * transmission of the last frame is complete. This is required when the USART is
		 * disabled or enters the Halt mode to avoid corrupting the last transmission */
	}
	USART1->ICR |= USART_ICR_TCCF; /* clear TC flag */
}

static void ReceiveChar(char charToReceive){
	/* First, check for the Null terminator */
	/*if(charToReceive == '\n'){
		AppendToRXBuffer('\0');
		/* Send each Value in RXBuffer to TDR Register */
		/*for(int i = 0; i <= Curr_Pointer_Location; i++){
			int CurrChar = Ret_Curr_Val_RXBuffer(i);
			TransmitChar(CurrChar);
		}
		ResetRXBuffer();
	}
	/* Second, check if the index is at max length */
	/*else if(Curr_Pointer_Location == MAX_LENGTH){
		ResetRXBuffer();
	}
	else{
		AppendToRXBuffer(charToReceive);
	}*/
	if(charToReceive == '\n'){
		AppendToRXBuffer('\0');
	}
	AppendToRXBuffer(charToReceive);
	USART1->TDR = charToReceive;
}

void USART1_IRQHandler(){
	uint8_t charToReceive = 0;
	//static uint16_t outputLength = sizeof(TXBuffer)/sizeof(TXBuffer[0]);

	/* Send Char Out */
	if((USART1->ISR & USART_ISR_TC) == USART_ISR_TC){
		/*for(int i = 1; i <= outputLength; i++){
			bytes_to_send = TXBuffer[i];
			USART1->TDR = bytes_to_send;
		}*/
		USART1->ICR |= USART_ICR_TCCF; /* clear TC flag */
	}
	/* Receive Char */
	else if((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE){
		charToReceive = (uint8_t)(USART1->RDR); /* Receive data, clear flag */
		ReceiveChar(charToReceive);
	}
	/* Disable USART1_IRQn */
	else{
		NVIC_DisableIRQ(USART1_IRQn);
	}
}

extern void USART_Transmit(char* Msg){
	for(int i = 0; i <= strlen(Msg); i++){
		USART1->TDR = Msg[i];
	}
	USART1->ICR |= USART_ICR_TCCF; /* clear TC flag */
}
