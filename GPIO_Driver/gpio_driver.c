#include "gpio_driver.h"

void delay(volatile unsigned int count) {
	while (--count)
		;
}

// startup function use to depict starting of system this blinks led for 3 times
void Boot_signal() {

	for (int i = 0; i < 3; i++) {
		GPIOA->BSRR |= (1U << 5);
		delay(50000);
		GPIOA->BSRR |= (1U << 21);
		delay(50000);
	}
}

// this function used for showing that system is alive
void Breathing_signal() {
	while (1) {
		GPIOA->ODR ^= (1U << 5);
		delay(500000);
	}
}

//this mode is used to depict that security alert is triggered
void SoS_Alert_signal() {
	//fast blinks
	if (~(GPIOC->IDR & (1U << 13))) {
		for (int i = 0; i < 3; i++) {
			GPIOA->BSRR |= (1U << 5);
			delay(500000);
			GPIOA->BSRR |= (1U << 21);
			delay(500000);
		}
		//short blinks
		for (int i = 0; i < 3; i++) {
			GPIOA->BSRR |= (1U << 5);
			delay(250000);
			GPIOA->BSRR |= (1U << 21);
			delay(250000);
		}
		//fast blinks
		for (int i = 0; i < 3; i++) {
			if (i == 0 ){
				delay(500000);
			}
			GPIOA->BSRR |= (1U << 5);
			delay(500000);
			GPIOA->BSRR |= (1U << 21);
			delay(500000);
		}
	}

}

int main() {

	// GPIOA Enabled
	RCC->AHB1ENR |= (1U << 0);
	//Set mode of pin5 that is LED
	GPIOA->MODER |= (1U << 10);
	GPIOA->MODER &= ~(1U << 11);
	// GPIOC Enabled
	RCC->AHB1ENR |= (1U << 2);
	//set pin13 to input mode mode
	GPIOC->MODER &= ~(1U << 26);
	GPIOC->MODER &= ~(1U << 27);

//	Breathing_signal();

	SoS_Alert_signal();



}
