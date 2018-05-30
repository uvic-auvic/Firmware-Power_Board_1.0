/*
 * INA226_Current_Sensor.h
 *
 *  Created on: May 29, 2018
 *      Author: Poornachander
 */

#ifndef INA226_CURRENT_SENSOR_H_
#define INA226_CURRENT_SENSOR_H_

extern void init_INA226_Current_Sensor();

extern uint16_t update_system_current();

extern uint16_t update_motor_current();

#endif /* INA226_CURRENT_SENSOR_H_ */
