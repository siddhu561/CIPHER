#include "stm32f446xx.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define GPIOA_EN 				(1U<<0)
#define USART2_EN 				(1U<<17)
#define transmitter 			(1U<<2)
#define reciver 				(1U<<3)
#define clck_freq    			16000000
#define budrate 				115200
#define USART2_EN_CR1     		(1U<<13)

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

	GPIOA->AFR[0] |= (1U << 8);
	GPIOA->AFR[0] |= (1U << 9);
	GPIOA->AFR[0] |= (1U << 10);
	GPIOA->AFR[0] &= ~(1U << 11);

	GPIOA->AFR[0] |= (1U << 12);
	GPIOA->AFR[0] |= (1U << 13);
	GPIOA->AFR[0] |= (1U << 14);
	GPIOA->AFR[0] &= ~(1U << 15);

	//enable usart module
	//enable clock of usart
	RCC->APB1ENR |= USART2_EN;

	//set usart direction
	USART2->CR1 = transmitter;
	USART2->CR1 |= reciver;

	//set usart budrate
	set_Budrate(clck_freq, budrate, USART2);
	//initate usart
	USART2 ->CR1 |= (1U<<13) ;
}

char UART_RECIVE() {

	while (!(USART2->SR & (1U << 5))) {
	}
	return USART2->DR;

}

void UART_TRANSMIT(char ch) {

	while (!(USART2->SR & (1U << 7))) {
	}
	USART2->DR = (ch & 0xFF);

}

void UART_TRANSMIT_STRING(char *str) {
	while (*str){
		UART_TRANSMIT(*str);
		str++;
	}
}

uint32_t calculate_bd(uint32_t perifclk, uint32_t Budrate) {
	return ((perifclk + (Budrate / 2)) / Budrate);
}

void set_Budrate(uint32_t perifclk, uint32_t Budrate, USART_TypeDef *USARTx) {
	USARTx->BRR = calculate_bd(perifclk, Budrate);

}


