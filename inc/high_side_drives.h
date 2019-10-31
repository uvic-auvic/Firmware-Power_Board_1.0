/*
 * high_side_drives.h
 *
 *  Created on: May 9, 2018
 *      Author: abates
 */

#ifndef HIGH_SIDE_DRIVES_H_
#define HIGH_SIDE_DRIVES_H_

typedef enum board_section{
	system_power,
	motor_power,
	_12V_9V_power,
	_5V_power,
}board_section_t;

typedef enum state{
	off = 0,
	on
}state_t;

extern void power_enable(board_section_t drive, state_t state);

extern void parallel_battries_enable(state_t state);

extern void init_HSDs();

#endif /* HIGH_SIDE_DRIVES_H_ */
