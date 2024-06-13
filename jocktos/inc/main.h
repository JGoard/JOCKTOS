/**
* \brief This module is to act as companion header for main.c
*/
#ifndef _MAIN_H_
#define _MAIN_H_
/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "jocktos.h"
// Middleware
// #include "stm32f303xe.h"
// #include "stm32f3xx_hal.h"
// Bios
// Standard C
#include <stdint.h>

/* -- Defines ------------------------------------------------------------- */
// #define B1_Pin GPIO_PIN_13
// #define B1_GPIO_Port GPIOC
// #define USART_TX_Pin GPIO_PIN_2
// #define USART_TX_GPIO_Port GPIOA
// #define USART_RX_Pin GPIO_PIN_3
// #define USART_RX_GPIO_Port GPIOA
// #define LD2_Pin GPIO_PIN_5
// #define LD2_GPIO_Port GPIOA
// #define TMS_Pin GPIO_PIN_13
// #define TMS_GPIO_Port GPIOA
// #define TCK_Pin GPIO_PIN_14
// #define TCK_GPIO_Port GPIOA
// #define SWO_Pin GPIO_PIN_3
// #define SWO_GPIO_Port GPIOB
/* -- Types --------------------------------------------------------------- */

/* -- Externs (avoid these for library functions) ------------------------- */

/* -- Function Declarations ----------------------------------------------- */

/**
* \brief Takes and gives a binary Semaphore
*
* Tests the semaphore synchonization mechanism 
*
* \return
*/
void mutexTestTask(uintptr_t* new_sp);

/**
 * @brief Burn clock cycles and occupy stack space
 * 
 * @param cycles O(cycles) complexity -> # of multiplications
 * @return int meaningless
 */
int burnCycles(int cycles);

/**
 * @brief Recursively occupy additional stack space.
 * 
 * For testing task stack overflow error handling
 * 
 * @param depth recursion depth
 * @param cycles multiplications between recursion calls and returns
 * @return int meaningless
 */
int inflateStack(int depth, int cycles);

/**
* \brief Task function that intentionally causes a process stack overflow
*
* For testing task stack overflow error handling
*
* \return
*/
void stackInflationTestTask(uintptr_t* new_sp);

void sleepTest(uintptr_t* new_sp);

int main(void);

#endif /* _MAIN_H_ */
