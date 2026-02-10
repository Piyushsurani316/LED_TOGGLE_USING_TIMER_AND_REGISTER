#include "stm32f1xx.h"

void TIM2_Init_ms(uint16_t delay_ms);
void clock_setup();

int main(void)
{
	clock_setup();
    TIM2_Init_ms(1000);
    while(1){}

}
 void TIM2_IRQHandler(void)
 {
     if (TIM2->SR & TIM_SR_UIF)
            {
                TIM2->SR &= ~TIM_SR_UIF;   // Clear update flag
                GPIOC->ODR ^= GPIO_ODR_ODR13;     // set and reset Bit 13 of ODR //togglr bit and so LED
            }
 }


void TIM2_Init_ms(uint16_t delay_ms)
   {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;


    //TIM2-> CR1 &= ~(0x01);
   // RCC->APB1RSTR |=  (RCC_APB1RSTR_TIM2RST);   /* Reset TIM4. */
      // RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM2RST);
    //RCC->APB1RSTR |= (0x01);
    TIM2->PSC = 7199;              // 0.1 ms tick
    TIM2->ARR = (delay_ms * 10) - 1;
   // TIM2->EGR |= TIM_EGR_UG;           // Load PSC + ARR
    TIM2->SR &= ~TIM_SR_UIF;                  // RESET counter (IMPORTANT)
    TIM2->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);
    TIM2->CR1 |= TIM_CR1_CEN;
   }

void clock_setup()
   {
	//Clock Setting
	    RCC->CR |= RCC_CR_HSEON; //bit 16 (High Speed External Clock ) ON of Clock Control Register
	    while (!(RCC->CR & RCC_CR_HSERDY)); // wait till external oscillator stable

	    //Flash Access
	    FLASH->ACR |= FLASH_ACR_LATENCY_2; // 2 wait cycle of CPU required for 72MHZ frequency to access flash
	    FLASH->ACR |= FLASH_ACR_PRFTBE; //to prefetch data from flash when cpu access other data

	    // PLL Setup
	    RCC->CFGR |= RCC_CFGR_PLLSRC;         // PLL source from external oscillator
	    RCC->CFGR &= ~RCC_CFGR_PLLMULL; //required setting otherwise after restart some issue
	    RCC->CFGR |= RCC_CFGR_PLLMULL9;       // PLL Multiply with 9 to get 72MHZ Clock

	    // APB1 Setup
	    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; //set apb1 prescaler 2 to get max 36 MHZ

	    // Enable PLL
	    RCC->CR |= RCC_CR_PLLON; //start PLL
	    while (!(RCC->CR & RCC_CR_PLLRDY)); //wait till PLL Frequency stable


	    RCC->CFGR |= RCC_CFGR_SW_PLL; //PLL configure as system clock
	    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); //wait till system clock = PLL

	    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;//GPIO PORTC clock enable

	    GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13); // clear port 13 old setting
	    GPIOC->CRH |=  GPIO_CRH_MODE13_1;     // Port13 2Mz Push pull output set
	}
