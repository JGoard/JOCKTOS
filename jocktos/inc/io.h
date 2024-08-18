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
/* Pull-up or pull-down resistor configurations */
#define JOCK_IO_NOPUPDR     0b00    // No pull up or pull down
#define JOCK_IO_PUR         0b01    // Pull up res
#define JOCK_IO_PDR         0b10    // Pull down res

/* GPIO mode configurations */
#define JOCK_IO_MODE_INPUT  0b00    // Input Mode
#define JOCK_IO_MODE_OUTPUT 0b01    // Output Mode
#define JOCK_IO_MODE_AF     0b10    // Alternate Function
#define JOCK_IO_MODE_ADC    0b11    // Analog Mode

/* GPIO output speed configurations */
#define JOCK_IO_OSPEED_LOW  0b00    // 2 MHz
#define JOCK_IO_OSPEED_MID  0b01    // 10 MHz
#define JOCK_IO_OSPEED_LOW2 0b10    // 2 MHz
#define JOCK_IO_OSPEED_HIGH 0b11    // 50 MHz

/* GPIO output type configurations */
#define JOCK_IO_OTYPE_PP    0b00    // 2 MHz
#define JOCK_IO_OTYPE_OD    0b01    // 10 MHz
/* -- Types --------------------------------------------------------------- */

/* -- Externs (avoid these for library functions) ------------------------- */

/* -- Function Declarations ----------------------------------------------- */

int16_t jock_io_init();
int16_t jock_io_initDigitalInput    (GPIO_TypeDef *port, uint8_t pin, uint8_t res);
int16_t jock_io_getDigitalInput     (GPIO_TypeDef *port, uint8_t pin, uint8_t* value);
int16_t jock_io_initDigitalOutput   (GPIO_TypeDef *port, uint8_t pin, uint8_t speed, uint8_t type);
int16_t jock_io_setDigitalOutput    (GPIO_TypeDef *port, uint8_t pin, uint8_t value);


#endif /* _IO_H_ */
