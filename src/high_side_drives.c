/*
 * high_side_drives.c
 *
 *  Created on: May 9, 2018
 *      Author: abates
 */
#include "high_side_drives.h"
#include "stm32f0xx.h"

extern void hsd_state(HSD drive, State state){
	uint16_t gpio = 0;
	if(drive == system_power){
		gpio = SYSTEM_POWER_GPIOA;
	}
	else if(drive == motor_power){
		gpio = MOTOR_POWER_GPIOA;
	}
	else if(drive == parallel_batteries){
		//We should do more checks on this before allowing
	}

	if(state == on){
		GPIOA->ODR |= gpio;
	}else{
		GPIOA->ODR &= (~gpio);
	}
}

extern void init_HSDs(){
	GPIO_InitTypeDef GPIOA_InitStruct;
	GPIOA_InitStruct.GPIO_Pin = (SYSTEM_POWER_GPIOA | MOTOR_POWER_GPIOA);
	GPIOA_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIOA_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
	GPIOA_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIOA_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIOA_InitStruct);
}
