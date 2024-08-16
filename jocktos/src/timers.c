/**
* \brief This module contains SysTick handling functions
*/
/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "os.h"
#include "sys.h"
#include "timers.h"
// Middleware
// Bios
// Standard C
#include <stdlib.h>

/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Local Globals (not for libraries with application instantiation) ---- */

/* -- Private Function Declarations --------------------------------------- */

void SysTick_Configuration(int freq) {
    __disable_irq(); // Disable interrupts to prevent any other interrupts from occurring
    SysTick->LOAD = (SystemCoreClock / freq) - 1; // Set the reload value for a 1ms interrupt
    SysTick->VAL = 0;                             // Clear the current value
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
    __enable_irq(); // Enable interrupts again
}

/* -- Public Functions----------------------------------------------------- */

/*
Ideas for Timer API Functions
    Create a timer that will run periodically
        Configurations include
            Input Capture mode
                Pulse width measurement
                Periodic or timing measurement (time between rising falling edges)
                Capturing time of an event
            Output Compare mode
            PWM Generation
            One-pulse mode output
    Attach Timer to a peripheral (e.g. PWM or ADC channel)
*/

void Timer2Init(TIM_TypeDef *TIMx, uint8_t irqPriority, uint16_t milliSeconds)
{
    // /* System Timer Configuration */
    /* TIM2 clock enable */
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    // Enable the interrupts
    NVIC_SetPriority(TIM2_IRQn, 0x03);
    NVIC_EnableIRQ(TIM2_IRQn);

    ///<TODO: Set up interrupt for timer 

    TIMx->CR1 &= ~(TIM_CR1_CEN);

    RCC->APB1RSTR |=  (RCC_APB1RSTR_TIM2RST);
    RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM2RST);

    TIMx->PSC   = SystemCoreClock / 1000;
    TIMx->ARR   = milliSeconds;
    // Send an update event to reset the timer and apply settings.
    TIMx->EGR  |= TIM_EGR_UG;
    // Enable the hardware interrupt.
    TIMx->DIER |= TIM_DIER_UIE;
    // Enable the timer.
    TIMx->CR1  |= TIM_CR1_CEN;

}

void TIM2_IRQHandler(void)
{
      // Handle a timer 'update' interrupt event
  if (TIM2->SR & TIM_SR_UIF) {
    TIM2->SR &= ~(TIM_SR_UIF);
    // Toggle the LED output pin.
    LED_PORT->ODR ^= (1 << LED_PIN);
  }
}