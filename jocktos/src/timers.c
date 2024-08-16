/**
* \brief This module contains SysTick handling functions
*/
/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "os.h"
#include "timers.h"
// Middleware
// Bios
#include "system_stm32f3xx.h"
#include "stm32f303xe.h"
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

void _testTimersInit()
{
    // /* TIM2 Configuration */
    // /* TIM2 clock enable */
    // RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    // /* Set the Timer prescaler to get 8MHz as counter clock */
    // Prescaler = (uint16_t) (SystemCoreClock / 8000000) - 1; TIM2->SMCR = RESET;
    // /* Reset the SMCR register */
    // #ifdef USE_ETR
    // /* Configure the ETR prescaler = 4 */
    // TIM2->SMCR |= TIM_ETRPRESCALER_DIV4 |
    // /* Configure the polarity = Rising Edge */
    // TIM_ETRPOLARITY_NONINVERTED |
    // /* Configure the ETR Clock source */
    // TIM_SMCR_ECE;
    // #else /* Internal clock source */
    // /* Configure the Internal Clock source */
    // TIM2->SMCR &= ~TIM_SMCR_SMS;
    // #endif /* USE_ETR */
    // TIM2->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
    // /* Select the up counter mode */

    // TIM2->CR1 |= TIM_COUNTERMODE_UP;
    // TIM2->CR1 &= ~TIM_CR1_CKD;
    // /* Set the clock division to 1 */
    // TIM2->CR1 |= TIM_CLOCKDIVISION_DIV1;
    // /* Set the Autoreload value */
    // TIM2->ARR = 100;
    // /* Set the Prescaler value */
    // TIM2->PSC = (SystemCoreClock / 8000000)-1;
    // /* Generate an update event to reload the Prescaler value immediately */
    // TIM2->EGR = TIM_EGR_UG;
    // TIM2->CCMR1 &= ~TIM_CCMR1_CC2S;
    // /* Connect the Timer input to IC2 */
    // TIM2->CCMR1 |= TIM_CCMR1_CC2S_0;
    // Input capture configuration
    // /* Enable the DMA1 */
    // DMA1_Channel7->CCR |= DMA_CCR_EN;
    // /* Enable the TIM Capture/Compare 2 DMA request */
    // TIM2->DIER |= TIM_DMA_CC2; /* Enables the TIM Capture Compare Channel 2
    // TIM2->CCER |= TIM_CCER_CC2E;
    // /* Enable the TIM2 */
    // TIM2->CR1 |= TIM_CR1_CEN;
    // /* wait until the transfer complete */
    // while ((DMA1->ISR & DMA_ISR_TCIF7) == RESET) {}
}
