/*
**	SysTick.c
**	This file have basic initialisation of NVIC with interrupt handling function
*/

/* IMPORTS */
#include "SysTick.h"
#include "tm4c123gh6pm.h"

/* FUNCTIONS */

/*
**	NVIC initialisation
**
**	To initialise NVIC, single element should be pass to this function.
** 
**  Argument period represents 24-bit reload value of NVIC system, 0x000000 - 0xFFFFFF
**
*/
void SysTick_Init(unsigned long period){
	/*All needed SysTick setting*/
	NVIC_ST_CTRL_R = 0;           // disable SysTick during setup
	NVIC_ST_RELOAD_R = period;     // reload value
	NVIC_ST_CURRENT_R = 0;        // any write to current clears it
	NVIC_SYS_PRI3_R = NVIC_SYS_PRI3_R & 0x00FFFFFF; // priority 0 
	NVIC_SYS_CTRL_R |= 0x02;
	NVIC_ST_CTRL_R = 0x00000003;  // enable with core clock and interrupts
}
