#include "stm32f4xx.h"
#include <stdint.h>
#define GPIOA_EN (1U<<0)
#define uart_enable (1U<<17)

void UART2_INIT(){
	/**configure usart gpio pin **/

	/*enable clk acces to gpio a */
	RCC->AHB1ENR |=GPIOA_EN;

	/*configure pa2 as alternate function */
	GPIOA->MODER &=~(1U<<4);
	GPIOA->MODER |=(1U<<5);

	/*configure alternate function mode*/
	GPIOA->AFR[0] |= (1U<<8);
	GPIOA->AFR[0] |= (1U<<9);
	GPIOA->AFR[0] |= (1U<<10);
	GPIOA->AFR[0] &=~ (1U<<11);

	/**configure usart modeule**/

	/*enable clk access to usartt*/
	RCC->APB1ENR |= uart_enable ;
	/*configure bud rate */
	/*set direction*/
	/*Enable usart */
}

static uint16_t compute_uart_div(uint32_t PeriPhCLK , uint32_t budrate){
	return  ((PeriPhCLK+(budrate/2))/budrate) ;
}


void main(){




	while(1) {

	}


}
