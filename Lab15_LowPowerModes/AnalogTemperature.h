#ifndef ANALOG_TEMP
#define ANALOG_TEMP

void Temp_Sensor_Init(void);
unsigned long ADC0_In(void);
float Convert_To_Cel (unsigned long sample);
void Print_Temp(void);

#endif
