/**
* \brief This module is the core JOCKTOS kernel functionality
*/
#ifndef _OS_H_
#define _OS_H_
/* -- Includes ------------------------------------------------------------ */
// Jocktos
// Middleware
// Bios
// Standard C
#include <stdint.h>
#include <stddef.h>
#include "tcb.h"


/* -- Defines ------------------------------------------------------------- */
#define CRITICAL_SECTION(...)                      \
    do {                                           \
        __asm volatile ("cpsid i" : : : "memory"); \
        arg;                                       \
        __asm volatile ("cpsie i" : : : "memory"); \
    } while (0)

/* -- Types --------------------------------------------------------------- */


//=============================================================================================

/** 
 * @brief Cortex-M4 Context Control Block
 */
typedef struct {
    volatile _Bool               active;    ///<    Initialization flag
    volatile T_TaskControlBlock* running;   ///<    Currently running task
    volatile T_TaskControlBlock* ready;     ///<    Singly linked list of tasks ready to run, in decending order of priority
    volatile T_TaskControlBlock* blocked;   ///<    Singly linked list of blocked tasks, in decending order of priority
    volatile T_TaskControlBlock* suspended; ///<    Singly linked list of suspended tasks, in decending order of priority
} T_Scheduler;

/* -- Externs (avoid these for library functions) ------------------------- */

/* -- Function Declarations ----------------------------------------------- */

/**
 * \brief Create a new task and add it to the scheduler.
 *
 * This function creates a new task by allocating memory for its stack, initializes the task stack pointer,
 * and inserts the task control block into the scheduler's ready queue.
 *
 * \param tcb Pointer to the task control block representing the new task.
 */
void createTask(T_TaskControlBlock* tcb);

/**
* \brief Switch the currently running task
*
* Updates the Schedulers linked lists and task states upon context switch 


#endif /* _OS_H_ */
