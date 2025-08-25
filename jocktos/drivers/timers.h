 /**
* \brief This header is to act as companion header for timers.c
*/
#ifndef _TIMERS_H_
#define _TIMERS_H_
/* -- Includes ------------------------------------------------------------ */
// Jocktos
// Middleware
#include "bsp.h"
// Bios
// Standard C
#include <stdint.h>

/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Externs (avoid these for library functions) ------------------------- */

/* -- Function Declarations ----------------------------------------------- */

void SysTick_Configuration(int freq);
void Timer2Init(TIM_TypeDef *TIMx, uint8_t irqPriority, uint16_t milliSeconds);

#endif /* _TIMERS_H_ */
