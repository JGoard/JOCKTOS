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

/* -- Types --------------------------------------------------------------- */

/**
 * \brief Struct for testing purposes.
 *
 * This struct is used for testing purposes in the main module.
 * It contains an integer value and a string ID.
 */
typedef struct {
    int value;    ///< Integer value.
    char ID[10];  ///< String ID.
} TestArgStruct;

/* -- Externs (avoid these for library functions) ------------------------- */

/* -- Function Declarations ----------------------------------------------- */

/**
 * \brief Task function that tests passing arguments to a task.
 *
 * This function demonstrates how to pass arguments to a task.
 * It takes a pointer to a `TestArgStruct` as its argument,
 * which contains an integer value and a string ID.
 *
 * \param arg Pointer to a `TestArgStruct` cast as a `void*`.
 *
 * \return
 */
void testArgsTask(void* arg);
/**
* \brief Takes and gives a binary Semaphore
*
* Tests the semaphore synchonization mechanism 
*
* \return
*/
void mutexTestTask(void* arg);

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
void stackInflationTestTask(void* arg);

void sleepTest(void* arg);

int main(void);

#endif /* _MAIN_H_ */
