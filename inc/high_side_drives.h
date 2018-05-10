/*
 * high_side_drives.h
 *
 *  Created on: May 9, 2018
 *      Author: abates
 */

#ifndef HIGH_SIDE_DRIVES_H_
#define HIGH_SIDE_DRIVES_H_

#define SYSTEM_POWER_GPIOA		(0x0800)
#define MOTOR_POWER_GPIOA		(0x1000)

typedef enum HSD{
	system_power,
	motor_power,
	parallel_batteries
}HSD;

typedef enum state{
	off = 0,
	on
}State;

extern void hsd_state(HSD drive, State state);

extern void init_HSDs();

#endif /* HIGH_SIDE_DRIVES_H_ */
