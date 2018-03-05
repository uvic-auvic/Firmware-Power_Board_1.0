#ifndef USART_H_
#define USART_H_

#include "stm32f0xx.h"

#define MAX_INPUT_DATA (12)
#define MAX_OUTPUT_DATA	(12)

char BuffTX[MAX_OUTPUT_DATA];
char BuffRX[MAX_INPUT_DATA];

extern void initUSART();

extern void TransmitChar(uint16_t Data);

extern void ReceiveChar(char Char);

#endif /* USART_H_ */
