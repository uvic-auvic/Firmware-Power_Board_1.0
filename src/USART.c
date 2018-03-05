#include "USART.h"
#include "stm32f0xx.h"

static int Curr_Array_Index = 0;

char BuffTX[MAX_OUTPUT_DATA]; //= "hello!\n\r"
char BuffRX[MAX_INPUT_DATA];

/* Append the Char Received to BuffRX*/
static void AppendToBuffRX(char Data){
	BuffRX[Curr_Array_Index] = Data;
	Curr_Array_Index++;
}

//PB6 and PB7
static void GPIO(){
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER |= GPIO_MODER_MODER6 | GPIO_MODER_MODER7;
}

//1 char = 8 bits
static void USART(){
	/* Enable Peripheral */
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	/* Baudrate set at 9600 Kbps */
	USART1->BRR = 480000 / 96;

	/* Enable Transmitter, Receiver, TC interrupt, RXNE interrupt, and USART*/
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_TCIE
				| USART_CR1_RXNEIE | USART_CR1_TXEIE | USART_CR1_UE;
		/* TXEIE will send an interrupt when the TDR register is empty*/

	/* polling idle frame Transmission */
	while((USART1->ISR & USART_ISR_TC) != USART_ISR_TC)
	{
		/* add time out here for a robust application */
	}
	USART1->ICR |= USART_ICR_TCCF;/* clear TC flag */

	//NVIC_EnableIRQ(USART1_IRQn);
	//NVIC_SetPriority(USART1_IRQn,1);
}

extern void TransmitChar(uint16_t Data){
	/* Start USART transmission */
	USART1->TDR = (Data & (uint16_t)0x01FF);/* Clears TXE bit */
	while((USART1->ISR & USART_ISR_TC) == 0){
		/*After writing the last data into the USART_TDR register, wait until TC=1. This indicates
		that the transmission of the last frame is complete. This is required for instance when
		the USART is disabled or enters the Halt mode to avoid corrupting the last
		transmission*/
		//Wait for complete transfer.
	}
	USART1->ICR |= USART_ICR_TCCF;/* clear TC flag */

}
extern void ReceiveChar(char Char){
	/*if(Char == '\n' || Char == '\r'){
		AppendToBuffRX('\0');
	}*/
	USART1->RDR = Char;
	char data = "";
	/* Reading the Data will clear the RXNE flag*/
	if ((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE)
	{
	 data = (uint16_t)(USART1->RDR & (uint16_t)0x01FF); /* Receive data, clear flag */
	}
	AppendToBuffRX(data);

}
extern void initUSART(){
	GPIO();
	USART();
}
/* IF THERE IS GOING TO BE AN ISSUE, IT MIGHT BE THE TXEIE BIT OR THE UINT16_t */
/* A frame : 1 start bit followed by 7,8, or 9 data bits (parity control inc. if enabled),
 * ending with a number of stop-bits.
 */
/*Character reception procedure
1. Program the M bit in USART_CR1 to define the word length. 			DONE:KEEPING TO 8-BIT CHAR
2. Select the desired baud rate using the baud rate register USART_BRR	DONE: 9600 Kbps
3. Program the number of stop bits in USART_CR2.						DONE:KEEPING 1
4. Enable the USART by writing the UE bit in USART_CR1 register to 1.	DONE
5. Select DMA enable (DMAR) in USART_CR3 if multibuffer communication is to take
	place. Configure the DMA register as explained in multibuffer communication.	NOT USING
6. Set the RE bit USART_CR															DONE
*/

/*Character transmission procedure
1. Program the M bit in USART_CR1 to define the word length.
2. Select the desired baud rate using the USART_BRR register.
3. Program the number of stop bits in USART_CR2.
4. Enable the USART by writing the UE bit in USART_CR1 register to 1.
5. Select DMA enable (DMAT) in USART_CR3 if multibuffer communication is to take
place. Configure the DMA register as explained in multibuffer communication.
6. Set the TE bit in USART_CR1 to send an idle frame as first transmission.
7. Write the data to send in the USART_TDR register (this clears the TXE bit). Repeat this
for each data to be transmitted in case of single buffer.
8.

During transmission: Transmission Complete, Clear to Send, Transmit data Register
empty interrupt.
 */
