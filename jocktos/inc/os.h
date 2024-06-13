/**
* \brief This header is to act as companion header for os.c
*/
#ifndef _OS_H_
#define _OS_H_
/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "tcb.h"
// Middleware
// Bios
// Standard C
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* -- Defines ------------------------------------------------------------- */

/** 
 * @brief enable / disablt ISR wrapper
 */
#define CRITICAL_SECTION(...)                      \
    do {                                           \
        __asm volatile ("cpsid i" : : : "memory"); \
        __VA_ARGS__;                               \
        __asm volatile ("cpsie i" : : : "memory"); \
    } while (0)

/** 
 * @brief TODO: should move this somewhere that isn't exposed to end user
 */
#define TRIGGER_PendSV *(uintptr_t volatile *)0xE000ED04 = (1U << 28)

#define T_JOCKTOSCONFIG_DEF(...) \
{                                \
    .enableMonitor      = false, \
    .enableMain         = false, \
    .enableIdle         = false, \
     __VA_ARGS__                 \
}
/* -- Types --------------------------------------------------------------- */

/** 
 * @brief Cortex-M4 Context Control Block
 */
typedef struct {
    volatile bool pending;
    volatile uint32_t tickCount;
    volatile T_TaskControlBlock* running;   ///<    Currently running task
    volatile T_TaskControlBlock* ready;     ///<    Singly linked list of tasks ready to run, in decending order of priority
    volatile T_TaskControlBlock* suspended; ///<    Singly linked list of suspended tasks, in decending order of priority
} T_Scheduler;

typedef struct {
    bool enableMonitor;
    bool enableIdle;
    bool enableMain;
} T_JocktosConfig;

/* -- Externs (avoid these for library functions) ------------------------- */

extern T_Scheduler JOCKTOSScheduler;

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
 * \param head Pointer to destination for current running task.
 */
void switchRunningTask(volatile T_TaskControlBlock** head);

/**
 * \brief Updates the task control blocks stackUsage
 * 
 * \param tcb Pointer to task control block to be monitored.
 */
static inline void monitorStackUsage(volatile T_TaskControlBlock** tcb) {
    (*tcb)->stackUsage = 100.0 * (1.0 - ((double)((*tcb)->u32TaskStackPointer \
    - (*tcb)->u32TaskStackOverflow)) / (double)((*tcb)->u32StackSize_By * sizeof(uintptr_t)));
}

/**
* \brief pre defined OS task for idle.
*
* Infinite while loop.
* TODO: Figure out how to low power sleep without disabling ISR's
*/
void idleJOCKTOS(uintptr_t* new_sp);

/**
 * \brief pre defined OS task to monitor stack usage
 */
void monitorJOCKTOS(uintptr_t* new_sp);

/**
 * \brief configure / enable built in OS tasks
 */
void configureJOCKTOS(T_JocktosConfig* config);

/**
 * \brief Enable scheduler and context switching ISR's
 *
 * Sets the priorities and enables systick and pendSV handlers
 *
 */
void runJOCKTOS(void);

/**
 * \brief returns the current OS tick count
 * 
 * unsigned 32 bit millisecond counter
 * 
 */
static inline uint32_t currentTime() { return JOCKTOSScheduler.tickCount; }

#endif /* _OS_H_ */
