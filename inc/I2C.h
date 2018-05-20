/*
 * I2C.h
 *
 *  Created on: May 15, 2018
 *      Author: Poornachander
 */

#ifndef I2C_H_
#define I2C_H_

#include "stm32f0xx.h"

extern void I2C_init();

extern void I2C_read();

extern void I2c_write(uint8_t address, uint8_t numBytes, uint8_t message[]);


#endif /* I2C_H_ */
