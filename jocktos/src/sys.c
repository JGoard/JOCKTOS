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
#include "stm32m4cortex_bsp.h"
// Standard C
#include <stdlib.h>

/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Local Globals (not for libraries with application instantiation) ---- */

/* -- Private Function Declarations --------------------------------------- */

/* -- Public Functions----------------------------------------------------- */


void jock_sys_LEDInit(void)
{
    // Enable clock for GPIO Port A and set the 0th pin (PA0) to be general purpose output mode
    RCC->AHBENR         |= RCC_AHBENR_GPIOAEN;  // Enable clock for GPIO Port A
    LED_PORT->MODER     |= GPIO_MODER_MODER5_0; // Set the 5th pin (PA5) to be general purpose output mode

    /* Set pin 5 of Port A to be a push-pull, low-speed output */
    /* Clear bits 0-1 of the MODER register to reset the pin to input mode */
    LED_PORT->MODER  &= ~(0x3 << (LED_PIN*2));
    /* Set bits 0-1 of the MODER register to set the pin to output mode */
    LED_PORT->MODER  |=  (0x1 << (LED_PIN*2));
    /* Clear the OTYPER bit to set the pin to push-pull mode */
    LED_PORT->OTYPER &= ~(1 << LED_PIN);
}

void jock_sys_LEDToggle(void)
{
    // Toggle the LED pin
    LED_PORT->ODR ^= (1 << LED_PIN);
}

void jock_sys_ErrorLED(void)
{
    ///<TODO: Pass in Error Value from JOCKTOS inits and use different conditions for cycling error states
}