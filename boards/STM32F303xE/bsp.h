/**
* \brief This header is to act as companion header for sys.c
*           It is intended to hold any M4 Cortex specific definitions for any relevant Nucleo board
*           The macro is intended to be defined at compile time to select a board through defining it in gcc
*/
#pragma once
/* -- Includes ------------------------------------------------------------ */
// Jocktos
// Middleware
///<TODO: include ifdefs here to distinguish between different boards
/* #if defined(STM32F303xE) */ 
#include "stm32f303xe.h"
/* #endif //STM32F303xE */ 

// Bios
// Standard C
#include <stdint.h>

/* -- Defines ------------------------------------------------------------- */
/* #if defined(STM32F303xE) */ 
#define LED_PIN     5
#define LED_PORT    GPIOA
/* #endif //STM32F303xE */ 

/* #if defined(STM32F403xE) */ 
// #define LED_PIN     1
// #define LED_PORT    GPIOB
/* #endif //STM32F403xE */ 

/* -- Types --------------------------------------------------------------- */

/* -- Externs (avoid these for library functions) ------------------------- */

/* -- Function Declarations ----------------------------------------------- */
