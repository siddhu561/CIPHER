#include "led_Patterns.h"
/**DELAY FUNCTION**/
void delay(int n) {
	n = n * 100000;
	volatile int i = 0;
	while (i < n) {
		i++;
	}
}
/*** Initiate PC13 ***/
void init_led() {
	RCC->AHB1ENR |= (1U << 2);
	GPIOC->MODER &= ~(1U << 27);
	GPIOC->MODER |= (1U << 26);
}
/**Pattern-1**/
void BOOT_OK() {
	for (int i = 0; i < 3; i++) {
		GPIOC->BSRR = (1U << 29);
		delay(1);
		GPIOC->BSRR = (1U << 13);
		delay(1);
	}

}
/**Pattern-2**/
void RUNNING() {
	for (;;) {
		GPIOC->BSRR = (1U << 29);
		delay(6);
		GPIOC->BSRR = (1U << 13);
		delay(6);
	}
}
/**Pattern-3**/
void SOS() {
	for (int i = 0; i < 3; i++) {
		GPIOC->BSRR = (1U << 29);
		delay(1);
		GPIOC->BSRR = (1U << 13);
		delay(1);
	}
	for (int i = 0; i < 3; i++) {
		GPIOC->BSRR = (1U << 29);
		delay(5);
		GPIOC->BSRR = (1U << 13);
		delay(5);
	}
	for (int i = 0; i < 3; i++) {
		GPIOC->BSRR = (1U << 29);
		delay(1);
		GPIOC->BSRR = (1U << 13);
		delay(1);
	}
}
/**Pattern-**/
void Error(){
	for (;;) {
			GPIOC->BSRR = (1U << 29);
			delay(1);
			GPIOC->BSRR = (1U << 13);
			delay(1);
		}

}
int main(){
	init_led();
}
