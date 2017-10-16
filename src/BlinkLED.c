#include "stm32f0xx.h"

static void initBlinkyLED_GPIO(){
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	GPIOC->MODER |= 0x50000;
}

extern void initBlinkyLED (){
	initBlinkyLED_GPIO();
}
//static means only visable inside file.
//extern means visable to other files.
