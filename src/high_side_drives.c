/*
 * high_side_drives.c
 *
 *  Created on: May 9, 2018
 *      Author: abates
 */
#include "high_side_drives.h"
#include "stm32f0xx.h"

#define SYSTEM_POWER_GPIO		(GPIO_Pin_11)
#define MOTOR_POWER_GPIO		(GPIO_Pin_12)
#define _12V_9V_GPIO			(GPIO_Pin_1)
#define _5V_GPIO				(GPIO_Pin_2)

extern void power_enable(board_section_t board_section, state_t state){
	uint32_t *GPIO_Address = 0;
	uint16_t GPIO_Pin = 0;
	if(board_section == system_power){
		GPIO_Address = &GPIOA->ODR;
		GPIO_Pin = SYSTEM_POWER_GPIO;
	}
	else if(board_section == motor_power){
		GPIO_Address = &GPIOA->ODR;
		GPIO_Pin = MOTOR_POWER_GPIO;
	}
	else if(board_section == _12V_9V_power){
		GPIO_Address = &GPIOC->ODR;
		GPIO_Pin = _12V_9V_GPIO;
	}
	else if(board_section == _5V_power) {
		GPIO_Address = &GPIOC->ODR;
		GPIO_Pin = _5V_GPIO;
	}

	if(state == on){
		*GPIO_Address |= GPIO_Pin;
	}else{
		*GPIO_Address &= (~GPIO_Pin);
	}
}

extern void parallel_battries_enable(state_t state) {
	//Do some check before enabling.
	//Voltage monitoring circuit and firmware should be confirmed working FIRST!!
}

extern void init_HSDs(){

	// GPIO Initialization
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOC, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = (SYSTEM_POWER_GPIO | MOTOR_POWER_GPIO);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = (_12V_9V_GPIO | _5V_GPIO);
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	//Default start-up state
	power_enable(motor_power, on);
	power_enable(system_power, on);
	power_enable(_5V_power, on);
	power_enable(_12V_9V_power, on);
}
