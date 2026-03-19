#ifndef GPIO_DRIVER_H_
#define GPIO_DRIVER_H_


#include "stm32f4xx.h"

/* Function Declarations */

void delay(volatile unsigned int count);
void Boot_signal(void);
void Breathing_signal(void);
void SoS_Alert_signal(void);



#endif
