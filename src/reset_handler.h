/*
 * reset_handler.h
 *
 *  Created on: Nov 28, 2019
 *      Author: Danielle
 */

#include "stm32f0xx.h"
#include <UART.h>

#ifndef RESET_HANDLER_H_
#define RESET_HANDLER_H_

void reset_handler();
void reset_handlerInit(char*);

#endif /* RESET_HANDLER_H_ */
