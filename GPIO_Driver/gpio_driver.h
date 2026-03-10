#ifndef GPIO_DRIVER_H_
#define GPIO_DRIVER_H_


#include "stm32f4xx.h"

/* Function Declarations */

void delay(volatile unsigned int count);
void gpio_init(void);
void boot_signal(void);
void breathing_signal(void);
void sos_alert(void);



#endif
