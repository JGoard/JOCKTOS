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
