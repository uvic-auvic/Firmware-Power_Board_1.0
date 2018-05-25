#ifndef FSM_H_
#define FSM_H_

#include <UART.h>
#include "stm32f0xx.h"
#include "Buffer.h"

Buffer_t UARTinputBuffer;

/* Public function prototypes -----------------------------------------------*/

void FSM_Init();
void FSM(void* dummy);

#endif // FSM_H_

