/**
* \brief This header is to act as companion header for timers.c
*/
#ifndef _TIMERS_H_
#define _TIMERS_H_
/* -- Includes ------------------------------------------------------------ */
// Jocktos
// Middleware
// Bios
// Standard C
#include <stdint.h>

/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Externs (avoid these for library functions) ------------------------- */

/* -- Function Declarations ----------------------------------------------- */

void SysTick_Configuration(int freq);

#endif /* _TIMERS_H_ */
