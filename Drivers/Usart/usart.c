#include "stm32f446xx.h"
#include "stdint.h"


#define GPIOA_EN 				(1U<<0)
#define USART2_EN 				(1U<<17)
#define transmitter 			(1U<<2)
#define reciver 				(1U<<3)

void UART_INIT() {
	// Enable pin for usart 
	//enable ahb1 
	RCC->AHB1ENR |= GPIOA_EN;
	//set mode to alternate
	//PA2
	GPIOA->MODER &= ~(1U << 4);
	GPIOA->MODER |= (1U << 5);

	//PA3
	GPIOA->MODER &= ~(1U << 6);
	GPIOA->MODER |= (1U << 7);
	//set alternate mode to usart 

	GPIOA->AFR |= (1U << 8);
	GPIOA->AFR |= (1U << 9);
	GPIOA->AFR |= (1U << 10);
	GPIOA->MODER &= ~(1U << 11);

	GPIOA->AFR |= (1U << 12);
	GPIOA->AFR |= (1U << 13);
	GPIOA->AFR |= (1U << 14);
	GPIOA->MODER &= ~(1U << 15);

	//enable usart module 
	//enable clock of usart 
	RCC->APB1ENR |= USART2_EN;
	
	//set usart direction 
	USART2->CR1 = transmitter;
	USART2->CR1|= reciver ; 
	
	//set usart budrate 
	
	//initate usart 
}

char UART_RECIVE() {

}

void UART_TRANSMIT() {

}

uint16_t calculate_bd(uint32_t perifclk ,uint_32_t Budrate){
	return perifclk + ()
}

