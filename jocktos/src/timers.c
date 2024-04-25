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
    // Configure SysTick to generate an interrupt every 1 ms
    CRITICAL_SECTION(
        SysTick->LOAD = (SystemCoreClock / freq) - 1; // Set the reload value for a 1ms interrupt
        SysTick->VAL = 0;                             // Clear the current value
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
    );
}

/* -- Public Functions----------------------------------------------------- */
