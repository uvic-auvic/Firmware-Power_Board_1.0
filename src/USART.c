#include "USART.h"
#include "stm32f0xx.h"

/* Definitions */
#define MIN_COMMAND_LENGTH (4)
#define MAX_COMMAND_LENGTH (5)
#define MAX_OUPUT_DATA (10)

/* Private Variables */
static uint8_t bytes_to_send = (void*)0;
char stringToSend[MAX_OUPUT_DATA] = "";
char stringReceived[MAX_COMMAND_LENGTH] = "";
int Curr_Pointer_Location = 0;

/* Macros */
static void ResetBuffer(){
	for(int i = 0; i<MAX_COMMAND_LENGTH; i++){
		stringReceived[i] = 0;
	}
	Curr_Pointer_Location = 0;
}

static void AppendToBuffer(char Data){
	stringReceived[Curr_Pointer_Location] = Data;
	Curr_Pointer_Location++;
}

// ----------------------------------------------------------------------
/*
 *  @brief  Configures the USART1 pins on GPIO PA10 PA9
             - PA10 = Receive
           	 - PA9 = Transmit
  * @param  None
  * @retval None
*/
static void GPIO(){
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER |= GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1;/* AF mode */
	GPIOA->AFR[1] |= 0x110; /* Enable AF1 */
}
/*
 *  @brief  Configures USART1
  * @param  None
  * @retval None
*/
static void USART(){
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
	GPIO();
	USART();
}

// ----------------------------------------------------------------------------------
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
	if(charToReceive == '\n'){
		AppendToBuffer('\0');
		if(Curr_Pointer_Location >= MIN_COMMAND_LENGTH){
			AppendToBuffer(charToReceive);
		}
		ResetBuffer();
	}
	/* Second, check if the index is at max length */
	else if(Curr_Pointer_Location == MAX_COMMAND_LENGTH){
		ResetBuffer();
	}
	else{
		AppendToBuffer(charToReceive);
	}
}

/* Handles USART1 interrupt requests */
void USART1_IRQHandler(void)
{
	uint8_t charToReceive;
	static uint8_t outputIndx = 0;
	int size_of_Buff = sizeof(stringToSend)/sizeof(stringToSend[0]);

	/* Send Out the String */
	if((USART1->ISR & USART_ISR_TC) == USART_ISR_TC){
		bytes_to_send--;
		outputIndx++;
		if(bytes_to_send == 0){
			USART1->ICR |= USART_ICR_TCCF; /* Clear transfer complete flag */
			outputIndx = 0;

			/* If Buff is not empty, Continue until it is */
			if(size_of_Buff != 0){
				int i = size_of_Buff;
				while(i > 0)
					TransmitChar(stringToSend[i]);
			}
		}
		else{
			/* Clear TC flag and Fill TDR with a new char */
			USART1->TDR = stringToSend[outputIndx];
		}
	}
	/* Else receive the string */
	else if((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE){
		charToReceive = (uint8_t)(USART1->RDR); /* Receive Data and Clear Flag */
		ReceiveChar(charToReceive);
	}
	/* If not top 2, error has occurred then turn off NVIC */
	else{
		NVIC_DisableIRQ(USART1_IRQn); /* Disable USART1_IRQn */
	}

}
