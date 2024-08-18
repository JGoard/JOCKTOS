/**
* \brief This header is to act as companion header for io_list.c
*/
#ifndef _IO_LIST_H_
#define _IO_LIST_H_
/* -- Includes ------------------------------------------------------------ */
// Jocktos
// Middleware
#include "stm32m4cortex_bsp.h"
// Bios
// Standard C
#include <stdint.h>

/* -- Defines ------------------------------------------------------------- */
#define MAX_INPUTS     80
#define MAX_OUTPUTS    80

#define DIG_IN_A6   6
#define DIG_OUT_B3  3

/* -- Types --------------------------------------------------------------- */

/* -- Externs (avoid these for library functions) ------------------------- */
extern InputMap     inputList[]; 
extern OutputMap    outputList[]; 
extern uint16_t     lengthofInputs;
extern uint16_t     lengthofOutputs;

/* -- Function Declarations ----------------------------------------------- */

#endif /* _IO_H_ */
