#include "stm32f1xx.h"

void delay_ms(uint32_t ms); // simple delay function define

int main(void)
{
   //Clock Setting
    RCC->CR |= RCC_CR_HSEON; //bit 16 (High Speed External Clock ) ON of Clock Control Register
    while (!(RCC->CR & RCC_CR_HSERDY)); // wait till external oscillator stable

    //Flash Access
    FLASH->ACR |= FLASH_ACR_LATENCY_2; // 2 wait cycle of CPU required for 72MHZ frequency to access flash
    FLASH->ACR |= FLASH_ACR_PRFTBE; //to prefetch data from flash when cpu access other data

    // PLL Setup
    RCC->CFGR |= RCC_CFGR_PLLSRC;         // PLL source from external oscillator
    RCC->CFGR |= RCC_CFGR_PLLMULL9;       // PLL Multiply with 9 to get 72MHZ Clock

    // APB1 Setup
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; //set apb1 prescaler 2 to get max 36 MHZ

    // Enable PLL
    RCC->CR |= RCC_CR_PLLON; //start PLL
    while (!(RCC->CR & RCC_CR_PLLRDY)); //wait till PLL Frequency stable


    RCC->CFGR |= RCC_CFGR_SW_PLL; //PLL configure as system clock
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); //wait till system clock = PLL



    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;//GPIO PORTC clock enable
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;  // TIMER2 clock enable

    TIM2->PSC = 7200 - 1;  // // 72MHz/7200=10kz=0.1ms per tick
    TIM2->ARR = 10 - 1;     // 10 ticks per overflow = 1 ms
    TIM2->CR1 |= TIM_CR1_CEN;  // Enable timer


    GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13); // clear port 13 old setting
    GPIOC->CRH |=  GPIO_CRH_MODE13_1;     // Port13 2Mz Push pull output set


    while (1)
    {
        GPIOC->ODR ^= GPIO_ODR_ODR13;     // set and reset Bit 13 of ODR //togglr bit and so LED
        delay_ms(1000); // 1 second delay
    }
}

void delay_ms(uint32_t ms)
{
	  for(uint32_t i = 0; i < ms; i++)
	    {
	        TIM2->CNT = 0;               // Reset counter
	        TIM2->SR &= ~TIM_SR_UIF;     // Clear update flag

	        while (!(TIM2->SR & TIM_SR_UIF)); // Wait for overflow
	    }

	}


