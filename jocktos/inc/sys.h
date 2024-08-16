/**
* \brief This header is to act as companion header for sys.c
*/
#ifndef _SYS_H_
#define _SYS_H_
/* -- Includes ------------------------------------------------------------ */
// Jocktos
// Middleware
#include "stm32m4cortex_bsp.h"
// Bios
// Standard C
#include <stdint.h>

/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Externs (avoid these for library functions) ------------------------- */

/* -- Function Declarations ----------------------------------------------- */
/**
 * @brief Initializes the LED on the Nucleo board.
 *
 * This function must be called before any of the other LED functions.
 */
void jock_sys_LEDInit(void);

/**
 * @brief Toggle the LED on the Nucleo board
 *
 * The LED is toggled on and off by calling this function.
 */
void jock_sys_LEDToggle(void);

/**
 * @brief Turn on the error LED on the Nucleo board
 *
 * This function is used to indicate a system error.
 */
void jock_sys_ErrorLED(void);


#endif /* _SYS_H_ */
