/*
** main.c
** 
** - Runs on TM4C123
** - UART runs at 115,200 baud rate 
**
*/

/* IMPORTS */

#include "tm4c123gh6pm.h"
#include <stdio.h>
#include "uart.h"
#include "AnalogTemperature.h"
#include "SysTick.h"
#include "Hibernate.h"

/* PROTOTYPES */
void WaitForInterrupt(void);  // Sleep-mode activation

/* FUNCTIONS */

/*
**	Delay function
**
**  Function simulate delay of the microcontroller
**
**  Argument ms represents the unsigned long number of milliseconds needed to be delayed
**
**  NOTE: Function assumes, that the speed of main oscillator is 16MHz
*/
void Delay(unsigned long ms){unsigned long time;
  time = 2667 * ms; // 1ms
  while(time){
   time--;
  }
}

/*
** Initialisation of the PF3 and PF1 ports of the Port F
**
*/
void PortF_Init(void){
	/* Port F initialization */
	/*
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x0A;           // allow changes to PF1 PF3       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0A;          // 5) make PF1 PF3 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x00;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x0A;          // 7) enable digital pins PF3-PF1  
	*/
	
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0  
}

/*
**	Convert analog value to Celsius
**
**  Function use expression from Tiva C Series TM4C123GH6PM datasheet (p. 813) 
**  
**	Argument sample represents the analog sample, that has been recieved 
**  from ADC0_In() functions
**
**  Function returns float value, that represents temperature of CPU in Celsius
*/
float Convert_To_Cel (unsigned long sample) {
	return (147.5 - ((75 * 3.3 * sample) / 4096));
}


/*
**	Print temperature in Celsius on TeXaS display
**
*/
void Print_Temp() {
	unsigned long result = 0;
	float celsius = 0.0f;
	
	GPIO_PORTF_DATA_R = 0x02;     // LED is red
	/*Read analog value from temperature sensor*/
	result = ADC0_In();
	GPIO_PORTF_DATA_R = 0x08;   // LED is green
	/*Convert analog value in Celsius degrees format*/
	celsius = Convert_To_Cel(result);
	GPIO_PORTF_DATA_R = 0x00;   // LED is off
	printf("Temperature in C: %.1f\n", celsius);
}

/*
**	NVIC interrupt handler
**
**	Function print CPU Temperature in the debbug console every 5 seconds
**
*/
void SysTick_Handler(void){ 
	Print_Temp();                   // Print Temperature
}

/*
** Main function, that have run mode active
*/
int main (void) {
	UART_Init();    		// initialize UART for printing
	PortF_Init();   		// initialize Port F for Green and Red LED
	Temp_Sensor_Init();		// initialize temperature sensort
	printf("\nStarting active mode programm\n");
	while (1) {
		Delay(2000);								// Simulate 2 seconds delay
		Print_Temp();								// Print temperature
	}
}

/*
** Main function, that have sleep mode active
*/
int main2 (void) {
	UART_Init();    				// initialize UART for printing
	PortF_Init();   				// initialize Port F for Green and Red LED
	Temp_Sensor_Init();  			// initialize temperature sensort
	SysTick_Init(8000000);  // initialize  NVIC interrupts for sleep mode
	printf("\nStarting sleep mode programm\n");
	WaitForInterrupt();       // Put microcontroller in sleep mode
	return 0;
}

/*
** Main function, that have hibernation mode active
*/
int main3 (void) {
	UART_Init();            	// initialize UART for printing
	PortF_Init();           	// initialize Port F for Green and Red LED
	Hibernate_Mode_Init();  	// initialize hibernation mode
	Temp_Sensor_Init();     	// initialize temperature sensort
	
	if (!(HIB_RIS_R&0x01)) printf("\nStarting hibernation mode programm\n");  // Check for Hibernation interrupt bit
	else printf("\nWaking up from hibernation\n");
	
	Delay(25);                  // Give 25ms delay for microcontroller to stabilize system after wakining up
	Print_Temp();								// Print temperature
	
	GPIO_PORTF_DATA_R |= 0x08;  // Turn on LED to acknowledge, that microcontroller is currently active
	Delay(5000);								// Simulate 5 seconds delay
	
	GPIO_PORTF_DATA_R &= ~0x08; // Turn off LED after 5 seconds delay
	Activate_Hibernate();				// Go to hibernation
	while (1) {	
	}
}
