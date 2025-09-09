/**
* \brief This header is to act as companion header for sys.c
*/
#pragma once
/* -- Includes ------------------------------------------------------------ */
// Jocktos
// Middleware
#include "bsp.h"
// Bios
// Standard C
#include <stdint.h>

/* -- Defines ------------------------------------------------------------- */
    // LED Error States
    #define NO_FAILURE 0
    #define MILD_FAILURE 1
    #define SEVERE_FAILURE 2
/* -- Types --------------------------------------------------------------- */

/* -- Externs (avoid these for library functions) ------------------------- */

/* -- Function Declarations ----------------------------------------------- */
/**
 * @brief Initializes the LED on the Nucleo board.
 *
 * This function must be called before any of the other LED functions.
 */
void jock_sys_led_Init(void);

/**
 * @brief Toggle the LED on the Nucleo board
 *
 * The LED is toggled on and off by calling this function.
 */
void jock_sys_led_toggle(void);

/**
 * @brief Toggle the error LED based on the error value.
 *
 * This function is used to toggle the error LED on the Nucleo board based on the error value.
 *
 * @param[in] errorValue
 *            The error value to be indicated by the error LED.
 */
void jock_sys_error_led(int16_t errorValue);
