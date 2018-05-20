/*
 * sensors.h
 *
 *  Created on: May 11, 2018
 *      Author: Poornachander
 */

#ifndef SENSORS_H_
#define SENSORS_H_

typedef enum battery {
	Left_Battery = 1,
	Right_Battery
} battery_t;

extern void init_Sensors();

extern uint16_t Get_Battery_Voltage(battery_t battery);

extern uint32_t Get_Battery_Current(battery_t battery);

extern uint32_t Get_System_Current();

extern uint32_t Get_Motors_Current();

extern uint16_t Get_External_Pressure();

extern uint16_t Get_Internal_Pressure();

extern uint16_t Get_Temperature();

extern uint16_t Get_Humidity();

extern uint16_t Get_Water_Sensor_Value();

#endif /* SENSORS_H_ */
