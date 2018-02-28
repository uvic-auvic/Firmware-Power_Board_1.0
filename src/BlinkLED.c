#include "stm32f0xx.h"

//initializes' the pins without pushing to main.
static void initBlinkyLED_GPIO(){
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	GPIOC->MODER |= GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0;
}
//turns pins on
extern void initBlinkyLED (){
	initBlinkyLED_GPIO();
}
//static means only visable inside file.
//extern means visable to other files.
