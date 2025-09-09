/**
* \brief This module contains SysTick handling functions
*/
/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "sys.h"
#include "os.h"
#include "timers.h"
// Middleware
// Bios
// Standard C
#include <stdlib.h>

/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Local Globals (not for libraries with application instantiation) ---- */
    static uint32_t previousTime= 0;

/* -- Private Function Declarations --------------------------------------- */

/* -- Public Functions----------------------------------------------------- */


void jock_sys_led_Init(void){
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

void jock_sys_led_toggle(void){
    // Toggle the LED pin
    LED_PORT->ODR ^= (1 << LED_PIN);
}

void jock_sys_error_led(int16_t errorValue){
    uint32_t currentTime = jock_os_get_time();

    uint32_t timeDifference = currentTime - previousTime;
        switch (errorValue){
        case NO_FAILURE:        /* Blinks at 1Hz */
            if (timeDifference > 100) {
                jock_sys_led_toggle();
                previousTime = currentTime;
            }
            break;
        case MILD_FAILURE:      /* Blinks at 5Hz */
            if (timeDifference > 20) {
                jock_sys_led_toggle();
                previousTime = currentTime;
            }
            break;
        case SEVERE_FAILURE:    /* Blinks at 10Hz */
            if (timeDifference > 10) {
                jock_sys_led_toggle();
                previousTime = currentTime;
            }
            break;
        default:
            break;
    }
    ///<TODO: Pass in Error Value from JOCKTOS inits and use different conditions for cycling error states
}