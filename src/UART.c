//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

#include "Buffer.h"

#include "FreeRTOS.h"
#include "task.h"

#include <string.h>
#include <UART.h>
#include <stdbool.h>

//Definitions
#define MIN_COMMAND_LENGTH (4)
#define MAX_COMMAND_LENGTH (6)

#define MAX_OUPUT_DATA (10)

/* Private variables ---------------------------------------------------------*/
TaskHandle_t UARTTaskToNotify = NULL;

static uint8_t bytes_to_send = 0;
char stringtosend[MAX_OUPUT_DATA] = "";

char chars_recv[MAX_COMMAND_LENGTH] = ""; //Using for storing the previous portion of the command
int curr_data_recv_idx = 0; //Storing how far we are in to the current command

Buffer_t outputBuffer;

static void ResetCommBuffer(void){
	int i=0;
	for(; i<MAX_COMMAND_LENGTH; i++){
		chars_recv[i] = 0;
	}
	curr_data_recv_idx=0;
}

static void AppendToCommBuffer(char data){
	chars_recv[curr_data_recv_idx] = data;
	curr_data_recv_idx++;
}

// ----------------------------------------------------------------------------

/**
  * @brief  This function :
             - Enables GPIO clock
             - Configures the USART1 pins on GPIO PB6 PB7
  * @param  None
  * @retval None
  */
static void Configure_GPIO_USART1(void)
{
  /* Enable the peripheral clock of GPIOA */
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

  /* GPIO configuration for USART1 signals */
  /* (1) Select AF mode (10) on PA9 and PA10 */
  /* (2) AF1 for USART1 signals */
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODER9|GPIO_MODER_MODER10))\
                 | (GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1); /* (1) */
  GPIOA->AFR[1] = (GPIOA->AFR[1] &~ (GPIO_AFRH_AFRH1 | GPIO_AFRH_AFRH2))\
                  | (1 << (1 * 4)) | (1 << (2 * 4)); /* (2) */
}

/**
  * @brief  This function configures USART1.
  * @param  None
  * @retval None
  */
static void Configure_USART1(void)
{
  /* Enable the peripheral clock USART1 */
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

  //RCC->CFGR3 |= RCC_CFGR3_USART1SW_1;

  /* Configure USART1 */
  /* (1) oversampling by 16, 115200 baud */
  /* (2) 8 data bit, 1 start bit, 1 stop bit, no parity, reception mode */
  USART1->BRR = 480000 / 1152; /* (1) */
  USART1->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_UE | USART_CR1_TE ; /* (2) */

  /* polling idle frame Transmission */
   while((USART1->ISR & USART_ISR_TC) != USART_ISR_TC)
   {
     /* add time out here for a robust application */
   }
   USART1->ICR |= USART_ICR_TCCF;/* clear TC flag */
   USART1->CR1 |= USART_CR1_TCIE;/* enable TC interrupt */

  /* Configure IT */
  /* (3) Set priority for USART1_IRQn */
  /* (4) Enable USART1_IRQn */
  NVIC_SetPriority(USART1_IRQn, 0); /* (3) */
  NVIC_EnableIRQ(USART1_IRQn); /* (4) */
}

extern void UART_init(){
	/* Store the handle of the calling task. */
	UARTTaskToNotify = xTaskGetCurrentTaskHandle();

	Buffer_init(&outputBuffer);

	//initialize the UART driver
	Configure_GPIO_USART1();
	Configure_USART1();
}

static void new_char_recv(char chartoreceive){
	// \n cases
	if(chartoreceive=='\n' || chartoreceive == '\r'){
		// We don't want to use the \r, less to think about later with perfect strings
		AppendToCommBuffer('\0');
		if(curr_data_recv_idx < MIN_COMMAND_LENGTH){
			//Send_to_Odroid("tiny\r\n");
		}else{
			// Step 1 add to the input buffer
			Buffer_add(&UARTinputBuffer, chars_recv, strlen(chars_recv));

			// Step 2 is notify the FSM task that the buffer is no longer empty
			/* At this point xTaskToNotify should not be NULL as a transmission was
			in progress. */
			configASSERT( UARTTaskToNotify != NULL );

			/* Notify the task that the transmission is complete. */
			xTaskNotifyGive( UARTTaskToNotify );
		}
		ResetCommBuffer();
	}
	//Too many characters without \n

	//No need to check \n in the statement as it will be checked by the above statement
	else if(curr_data_recv_idx == MAX_COMMAND_LENGTH){
		//Send_to_Odroid("long\r\n");
		ResetCommBuffer();
	}else{
		AppendToCommBuffer(chartoreceive);
	}
}

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles EXTI 0 1 interrupt request.
  * @param  None
  * @retval None
  */

/**
  * @brief  This function handles USART1 interrupt request.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{

  GPIOC->ODR |= GPIO_Pin_8; //For debugging if interrupt is getting stuck

  uint8_t chartoreceive = 0;
  static uint8_t output_idx = 0;


  //Sending out the string
  if((USART1->ISR & USART_ISR_TC) == USART_ISR_TC)
    {
	  //The reason why we update the output_idx before code is because this happens
	  //after the first transfer is complete as the first transfer is loaded in else where
	  bytes_to_send--;
	  output_idx++;
      if(bytes_to_send == 0)
      {
        USART1->ICR |= USART_ICR_TCCF; /* Clear transfer complete flag */
        output_idx = 0;
        //if the buffer is not empty pop the next item off and keep going
        if(outputBuffer.size != 0){
        	bytes_to_send = Buffer_pop(&outputBuffer, stringtosend);
        	USART1->TDR = stringtosend[0]; /* Will initialize TC if TXE */
        }
      }
      else
      {
        /* clear transfer complete flag and fill TDR with a new char */
        USART1->TDR = stringtosend[output_idx];
      }
    }

  else if((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE)
  {
    chartoreceive = (uint8_t)(USART1->RDR); /* Receive data, clear flag */
    new_char_recv(chartoreceive);
  }
  else
  {
          NVIC_DisableIRQ(USART1_IRQn); /* Disable USART1_IRQn */
  }

  GPIOC->ODR &= ~(GPIO_Pin_8); //For debugging if interrupt is getting stuck
}

extern void UART_push_out(char* mesg){
	if(bytes_to_send > 0){
		Buffer_add(&outputBuffer, mesg, strlen(mesg));
	}
	else{
		bytes_to_send = strlen(mesg);
		strcpy(stringtosend, mesg);
		/* start USART transmission */
		USART1->TDR = stringtosend[0]; /* Will initialize TC if TXE */
	}
}

extern void UART_push_out_len(char* mesg, int len){
	if(bytes_to_send > 0){
		Buffer_add(&outputBuffer, mesg, len);
	}
	else{
		bytes_to_send = len;
		memcpy(stringtosend, mesg, len);
		/* start USART transmission */
		USART1->TDR = stringtosend[0]; /* Will initialize TC if TXE */
	}
}

/**
  * @}
  */

// ----------------------------------------------------------------------------
