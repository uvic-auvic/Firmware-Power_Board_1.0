/*
 * INA226_Current_Sensor.c
 *
 *  Created on: May 29, 2018
 *      Author: Poornachander
 */

#include "stm32f0xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "I2C.h"

#define I2C_TIMEOUT		2000

//INA226 Current and Power Sensor
#define CONFIG_REG_ADDRESS	0x00
#define CALIB_REG_ADDRESS	0x05
#define CURRENT_REG_ADDRESS	0x04
#define POWER_REG_ADDRESS	0x03

#define MOTOR_SENSOR_I2C_ADDRESS	0b01001111
#define SYSTEM_SENSOR_I2C_ADDRESS	0b01001010

#define CALIB_REG_VALUE	0x10AB //4mA per bit

#define TO_CURRENT(x) (x)  //Still needs to be defined

void init_INA226_Current_Sensor() {

	uint32_t valueToWriteOnStartup = (CALIB_REG_ADDRESS << 16) | CALIB_REG_VALUE;

	xSemaphoreTake(I2C_mutex, I2C_TIMEOUT);

	I2C_write(SYSTEM_SENSOR_I2C_ADDRESS, 3, (uint8_t *)&valueToWriteOnStartup);
	ulTaskNotifyTake(pdTRUE, I2C_TIMEOUT);

	I2C_write(MOTOR_SENSOR_I2C_ADDRESS, 3, (uint8_t *)&valueToWriteOnStartup);
	ulTaskNotifyTake(pdTRUE, I2C_TIMEOUT);

	xSemaphoreGive(I2C_mutex);
}

uint16_t update_system_current() {

	xSemaphoreTake(I2C_mutex, I2C_TIMEOUT);

	uint8_t currentReg = CURRENT_REG_ADDRESS;

	I2C_write(SYSTEM_SENSOR_I2C_ADDRESS, 1, &currentReg);
	ulTaskNotifyTake(pdTRUE, I2C_TIMEOUT);

	uint16_t current = 0;

	I2C_read(SYSTEM_SENSOR_I2C_ADDRESS, 2, &current);
	ulTaskNotifyTake(pdTRUE, I2C_TIMEOUT);

	xSemaphoreGive(I2C_mutex);

	current = TO_CURRENT(current);

	return current;
}

uint16_t update_motor_current() {
	xSemaphoreTake(I2C_mutex, I2C_TIMEOUT);

	uint8_t currentReg = CURRENT_REG_ADDRESS;

	I2C_write(MOTOR_SENSOR_I2C_ADDRESS, 1, &currentReg);
	ulTaskNotifyTake(pdTRUE, I2C_TIMEOUT);

	uint16_t current = 0;

	I2C_read(MOTOR_SENSOR_I2C_ADDRESS, 2, &current);
	ulTaskNotifyTake(pdTRUE, I2C_TIMEOUT);

	xSemaphoreGive(I2C_mutex);

	current = TO_CURRENT(current);

	return current;
}
