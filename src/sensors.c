/*
 * sensors.c
 *
 *  Created on: May 11, 2018
 *      Author: Poornachander
 */

#include "stm32f0xx.h"
#include "sensors.h"
#include "ADC.h"

//ADC is 12-bit right aligned
//Defines for calculations
#define ADC_TO_PIN_VOLTAGE (3.3/4095)

#define ADC_VALUE_TO_BAT_VOLTAGE	(10090 * ADC_TO_PIN_VOLTAGE) //mV

#define ADC_VALUE_TO_CURRENT	((float)121000/4095) //121000mA of current when pin is at 3.3V

// psi = (2500/819)*ADC_VALUE + 220  for 3.3V supply to sensor
// psi = (2500/1241)*ADC_VALUE + 220  for 5V supply to sensor
#define ADC_VALUE_TO_PRESSURE(x)	(((float)2500/1241)*(x) + 220) //10^(-2) psi, hecto-psi

// Returns value in mV
extern uint16_t Get_Battery_Voltage(battery_t battery) {
	//It would be nice to have decimal places
	uint16_t voltage = 0;

	if(battery == Left_Battery) {
		voltage = Get_ADC_Channel(ADC_Left_Bat_Voltage);
	} else if (battery == Right_Battery) {
		voltage = Get_ADC_Channel(ADC_Right_Bat_Voltage);
	}

	voltage = ADC_VALUE_TO_BAT_VOLTAGE * voltage; //Could be different from actual value due to variation is resistance.
	return voltage;
}

//Returns value in mA
extern uint16_t Get_Battery_Current(battery_t battery) {
	//It would be nice to have decimal places
	uint16_t current = 0;

	if(battery == Left_Battery) {
		current = Get_ADC_Channel(ADC_Left_Bat_Current);
	} else if (battery == Right_Battery) {
		current = Get_ADC_Channel(ADC_Right_Bat_Current);
	}

	current = ADC_VALUE_TO_CURRENT * current;
	return current;
}

//Returns value in mA
extern uint16_t Get_System_Current() {
	//This uses an I2C device
	//Returns constant for now so software can start testing

	return 0x00FF;
}

//Returns value in mA
extern uint16_t Get_Motors_Current() {
	//This uses an I2C device
	//Returns constant for now so software can start testing

	return 0x00FF;
}

extern uint16_t Get_External_Pressure() {
	uint16_t pressure = Get_ADC_Channel(ADC_Pressure_Sensor);
	pressure = ADC_VALUE_TO_PRESSURE(pressure);
	return pressure;
}

extern uint16_t Get_Internal_Pressure() {
	//This uses an I2C device
	//Returns constant for now so software can start testing

	return 0x00FF;
}

extern uint16_t Get_Temperature() {
	//This uses an I2C device
	//Returns constant for now so software can start testing

	return 0x00FF;
}

extern uint16_t Get_Humidity() {
	//This uses an I2C device
	//Returns constant for now so software can start testing

	return 0x00FF;
}

extern uint16_t Get_Water_Sensor_Value() {
	return Get_ADC_Channel(ADC_Water_Sensor); //Very simple return for now. Functionality can be expanded later as needed.
}
