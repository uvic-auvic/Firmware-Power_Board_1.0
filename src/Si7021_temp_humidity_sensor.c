#include "stm32f0xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "I2C.h"
#include "semphr.h"

//Time out
#define I2C_TIMEOUT          2000

//Si7021 register commands
#define CMD_MEASURE_RH_HM            0xE5
#define CMD_MEASURE_RH_NM            0xF5
#define CMD_MEASURE_TEMP_HM          0xE3
#define CMD_MEASURE_TEMP_NH          0xF3
#define CMD_READ_TEMP_PREV_RH        0xE0
#define CMD_RESET                    0xFE


//Si7021 I2C slave addresses
#define AD_READ         0b1000000
#define AD_WRITE        0b1000000


//Take the relative humidity and return it as a percentage
extern uint16_t Update_Humidity() {

	uint8_t humidityAddress = CMD_MEASURE_RH_HM;

	xSemaphoreTake(I2C_mutex, I2C_TIMEOUT);

	I2C_write(AD_WRITE, 1, &humidityAddress);
	ulTaskNotifyTake(pdTRUE, I2C_TIMEOUT);

	uint16_t relativeHumidity = 0;

	I2C_read(AD_READ, 2, &relativeHumidity);
	ulTaskNotifyTake(pdTRUE, I2C_TIMEOUT);

	xSemaphoreGive(I2C_mutex);

	relativeHumidity = switch_endiness_uint16(relativeHumidity);
	relativeHumidity = ((125*relativeHumidity)/65536)-6;

	return relativeHumidity; //returns relative humiditiy %

}

extern uint16_t Update_Temperature() {

	uint8_t tempAddress = CMD_MEASURE_TEMP_HM;

	xSemaphoreTake(I2C_mutex, I2C_TIMEOUT);
	I2C_write(AD_WRITE, 1, &tempAddress);

	ulTaskNotifyTake(pdTRUE, I2C_TIMEOUT);
	uint16_t temperature = 0;

	I2C_read(AD_READ, 2, &temperature);
	ulTaskNotifyTake(pdTRUE, I2C_TIMEOUT);

	xSemaphoreGive(I2C_mutex);

	temperature = switch_endiness_uint16(temperature);
	temperature = ((1757.2*temperature)/65536)+2263;

	return temperature; //returns 10*temperature in Kelvins

}
