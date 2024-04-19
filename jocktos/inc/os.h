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
        __VA_ARGS__;                               \
        __asm volatile ("cpsie i" : : : "memory"); \
    } while (0)

#define TRIGGER_PendSV *(uint32_t volatile *)0xE000ED04 = (1U << 28)
/* -- Types --------------------------------------------------------------- */


//=============================================================================================

/** 
 * @brief Cortex-M4 Context Control Block
 */
typedef struct {
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
*
* \return
*/
void switchRunningTask(void);

/**
* \brief Updates the task control blocks stackUsage
* \return
*/
static inline void monitorStackUsage(volatile T_TaskControlBlock** tcb) {
    (*tcb)->stackUsage = 100.0 * (1.0 - ((double)((*tcb)->u32TaskStackPointer \
    - (*tcb)->u32TaskStackOverflow)) / (double)((*tcb)->u32StackSize_By * sizeof(uint32_t)));
}
/**
* \brief Enable scheduler and context switching ISR's
*
* Sets the priorities and enables systick and pendSV handlers
*
* \return
*/
void runJOCKTOS(void);

#endif /* _OS_H_ */
