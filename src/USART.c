#include "USART.h"
#include "stm32f0xx.h"

//PB6 and PB7
static void GPIO(){
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER |= GPIO_MODER_MODER6 | GPIO_MODER_MODER7;
	//by default, the AFR is set to 0 (AF0) therefore, the USART_TX and RX is already enabled
}
/*
 * is this going to be continuously running, because if not, then i will need to
 * turn multiple things off before i can turn the UE bit to 0.
 *
 * the default is Asynchronous mode (frame format)
 * A frame : one start bit that is followed by 7,8, or 9 data bits (if
 * parity control is enabled then it is included in the bit count), ending with a number of
 * stop-bits.
 */

static void USART(){
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	//BAUDRATE IS AUTOMATIC
	USART1->CR2 |= USART_CR2_ABREN;

	USART1->CR1 |= (USART_CR1_TE | USART_CR1_RE | USART_CR1_TCIE) | USART_CR1_UE;
}
/*Character reception procedure
1. Program the M bit in USART_CR1 to define the word length.
2. Select the desired baud rate using the baud rate register USART_BRR
3. Program the number of stop bits in USART_CR2.
4. Enable the USART by writing the UE bit in USART_CR1 register to 1.
5. Select DMA enable (DMAR) in USART_CR3 if multibuffer communication is to take
place. Configure the DMA register as explained in multibuffer communication.
6. Set the RE bit USART_CR*/

static void DMA(){
	USART1->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR;
}
extern void initUSART(){
	GPIO();
	USART();
	DMA();
}
/*
 * By default, the signal (TX or RX) is in low state during the start bit. It is in high state during
the stop bit
 */
