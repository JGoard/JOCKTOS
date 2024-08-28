/**
* \brief This header is to act as companion header for io_list.c
*/
#ifndef _COMM_H_
#define _COMM_H_
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
void jock_comm_uartInit(GPIO_TypeDef *port, uint8_t pin, jockIoOutputType type, jockIoOutputSpeed speed, uint8_t altf);
#endif /* _COMM_H_ */
