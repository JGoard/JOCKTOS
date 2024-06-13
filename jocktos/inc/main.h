/**
* \brief This module is to act as companion header for main.c
*/
#ifndef _MAIN_H_
#define _MAIN_H_
/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "jocktos.h"
// Middleware
// Bios
// Standard C
#include <stdint.h>

/* -- Defines ------------------------------------------------------------- */

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
