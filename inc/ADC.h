#ifndef ADC_H_
#define ADC_H_

#define BUFFER_SIZE 8

enum ADC_Channels{
	ADC_Water_Sensor = 0,
	ADC_Pressure_Sensor = 1,
	ADC_Left_Bat_Voltage = 2,
	ADC_Right_Bat_Voltage = 3,
	ADC_Right_Bat_Current = 4,
	ADC_Left_Bat_Current = 5,
	ADC_Motor_Current_Alert = 6,
	ADC_System_Current_Alert = 7,
};

uint16_t ADC_Buffer[BUFFER_SIZE];

extern uint16_t Get_ADC_Channel(enum ADC_Channels channel);

extern void initADC();
#endif
