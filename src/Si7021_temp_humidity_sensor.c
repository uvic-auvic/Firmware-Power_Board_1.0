#include "stm32f0xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "I2C.h"
#include "semphr.h"

//Time out
#define I2C_TIMEOUT                  2000

//Si7021 register commands
#define CMD_MEASURE_RH_HM            0xE5
#define CMD_MEASURE_RH_NM            0xF5
#define CMD_MEASURE_TEMP_HM          0xE3
#define CMD_MEASURE_TEMP_NH          0xF3
#define CMD_READ_TEMP_PREV_RH        0xE0
#define CMD_RESET                    0xFE

//Temperature and Humidity Thresholds

#define temperatureThreshold (325)
#define humidityThreshold (75)


//Si7021 I2C slave addresses
#define Si_Address                   0b1000000

//initializes Humidity and Temperature monitoring tasks
void vHumidityTemperatureTaskInit (void){
	   xTaskCreate(monitorTemperature,
			(const char *)"monitorTemperature",
			configMINIMAL_STACK_SIZE,
			NULL,                 // pvParameters
			tskIDLE_PRIORITY + 1, // uxPriority
			NULL              ); // pvCreatedTask */
	   xTaskCreate(monitorHumidity,
			(const char *)"monitorHumidity",
			configMINIMAL_STACK_SIZE,
			NULL,                 // pvParameters
			tskIDLE_PRIORITY + 1, // uxPriority
			NULL              ); // pvCreatedTask */
}

//monitors Humidity and outputs a warning if its above a certain threshold
void monitorHumidity(void *dummy){
	while(1){
		if (Update_Humidity() >=  humidityThreshold ){
			UART_push_out("Humidity too high!");
			vTaskDelay(500);
		}
	}
}

//monitors Temperature and outputs a warning if its above a certain threshold
void monitorTemperature(void *dummy){
	while(1){
		if(Update_Temperature() >= temperatureThreshold){
			UART_push_out("Temperature too high!");
			vTaskDelay(500);
		}
	}
}


//Take the relative humidity and return it as a percentage
extern uint16_t Update_Humidity() {
	if (xSemaphoreTake(I2C_mutex, I2C_TIMEOUT)== pdTRUE) {

		uint8_t humidityAddress = CMD_MEASURE_RH_HM;

		I2C_write(Si_Address , 1, &humidityAddress);
		if(ulTaskNotifyTake(pdTRUE, I2C_TIMEOUT) == 0) {
			xSemaphoreGive(I2C_mutex);
			return 0xFFFF;
		}

		uint16_t relativeHumidity = 0;

		I2C_read(Si_Address, 2, (uint8_t *)&relativeHumidity);
		if(ulTaskNotifyTake(pdTRUE, I2C_TIMEOUT) == 0) {
			xSemaphoreGive(I2C_mutex);
			return 0xFFFF;
		}

		xSemaphoreGive(I2C_mutex);

		relativeHumidity = switch_endiness_uint16(relativeHumidity);
		relativeHumidity = ((125*relativeHumidity)/65536)-6;

		return relativeHumidity; //returns relative humidity %
	} else {
		return 0xFFFF;
	  }
}
// take the temperature in Kelvins and return it as 10*(actual temperature)
extern uint16_t Update_Temperature() {

	if (xSemaphoreTake(I2C_mutex, I2C_TIMEOUT)== pdTRUE) {

		uint8_t tempAddress = CMD_MEASURE_TEMP_HM;

		I2C_write(Si_Address, 1, &tempAddress);
		if(ulTaskNotifyTake(pdTRUE, I2C_TIMEOUT) == 0) {
			xSemaphoreGive(I2C_mutex);
			return 0xFFFF;
		}

		uint16_t temperature = 0;

		I2C_read(Si_Address, 2, (uint8_t *)&temperature);
		if(ulTaskNotifyTake(pdTRUE, I2C_TIMEOUT) == 0) {
			xSemaphoreGive(I2C_mutex);
			return 0xFFFF;
		}

		xSemaphoreGive(I2C_mutex);

		temperature = switch_endiness_uint16(temperature);
		temperature = ((1757.2*temperature)/65536)+2263;

		return temperature; //returns 10*temperature in Kelvins
	} else {
		return 0xFFFF;
	  }
}

// return humidity
//returns temperature
extern uint32_t return_temperature_value(){
	return 0b11010;
}
