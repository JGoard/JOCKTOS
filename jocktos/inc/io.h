/**
* \brief This header is to act as companion header for adc.c
*/
#ifndef _IO_H_
#define _IO_H_
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

int16_t jock_io_init();
int16_t jock_io_initDigitalInput    (GPIO_TypeDef *port, uint8_t pin, uint8_t res);
int16_t jock_io_getDigitalInput     (GPIO_TypeDef *port, uint8_t pin, uint8_t* value);
int16_t jock_io_initDigitalOutput   (GPIO_TypeDef *port, uint8_t pin, uint8_t speed, uint8_t type);
int16_t jock_io_setDigitalOutput    (GPIO_TypeDef *port, uint8_t pin, uint8_t* value);


#endif /* _IO_H_ */
