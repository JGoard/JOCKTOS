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
 * @brief Default JOCKTOS configuration
 */
#define JOCKTOSCONFIG_DEF(...)      \
{                                   \
    .enable_monitor      = false,   \
    .enable_main         = false,   \
    .enable_idle         = false,   \
    .allocator_block_size = 256,    \
     __VA_ARGS__                    \
}
/* -- Types --------------------------------------------------------------- */

/** 
 * @brief Cortex-M4 Context Control Block
 */
typedef struct {
    volatile bool pending;
    volatile uint32_t tick_count;
    volatile TaskControlBlock* running;   ///<    Currently running task
    volatile TaskControlBlock* ready;     ///<    Singly linked list of tasks ready to run, in decending order of priority
    volatile TaskControlBlock* suspended; ///<    Singly linked list of suspended tasks, in decending order of priority
} Scheduler;

/**
 * @brief Configuration settings for JOCKTOS allocator and built in tasks
 */
typedef struct {
    bool enable_monitor;         ///< Enable or disable monitoring
    bool enable_idle;            ///< Enable or disable idle task
    bool enable_main;            ///< return execution after enabling, with `main` considered a new task
    size_t allocator_block_size; ///< Size of the allocator block
} JocktosConfig;

/* -- Externs (avoid these for library functions) ------------------------- */

extern Scheduler JOCKTOSScheduler;

/* -- Function Declarations ----------------------------------------------- */

/**
 * \brief Create a new task and add it to the scheduler.
 *
 * This function creates a new task by allocating memory for its stack, initializes the task stack pointer,
 * and inserts the task control block into the scheduler's ready queue.
 *
 * \param tcb Pointer to the task control block representing the new task.
 */
void jock_createTask(TaskControlBlock* tcb);

/**
 * \brief Switch the currently running task
 *
 * Updates the Schedulers linked lists and task states upon context switch 
 *
 * \param head Pointer to destination for current running task.
 */
void switchRunningTask(volatile TaskControlBlock** head);

/**
 * \brief configure / enable built in OS tasks
 */
void jock_configure(JocktosConfig* config);

/**
 * \brief Enable scheduler and context switching ISR's
 *
 * Sets the priorities and enables systick and pendSV handlers
 *
 */
void jock_run(void);

/**
 * \brief returns the current OS tick count
 * 
 * unsigned 32 bit millisecond counter
 * 
 */
static inline uint32_t jock_currentTime() { return JOCKTOSScheduler.tick_count; }

#endif /* _OS_H_ */
