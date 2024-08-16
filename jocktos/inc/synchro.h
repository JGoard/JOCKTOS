/**
* \brief This header is to act as companion header for synchro.c
*/
#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_
/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "tcb.h"
// Middleware
// Bios
// Standard C
#include <stdint.h>

/* -- Defines ------------------------------------------------------------- */

/**
 * @brief default semaphore is binary (mutex)
 */
#define SEMAPHORE_DEF(...)      \
{   /* ---Internal Data---*/    \
    .value_           = 1,      \
    .count            = 2,      \
    .pending_queue_ = NULL,     \
    .owners_priority_  = 0,     \
     __VA_ARGS__                \
}

/* -- Types --------------------------------------------------------------- */

/**
 *  @brief Represents a semaphore for controlling access to a shared resource. 
 */
typedef struct {
    uint16_t value_;                                ///< [INTERNAL] current value
    uint16_t count;                                 ///< Queue size for lock instance
    volatile TaskControlBlock *pending_queue_;      ///< [INTERNAL] Linked list of tasks awaiting lock
    uint8_t owners_priority_;                       ///< [INTERNAL] TODO: prevent priority inversion
} Semaphore;

/* -- Externs (avoid these for library functions) ------------------------- */

/* -- Function Declarations ----------------------------------------------- */

/**
 * @brief Takes a semaphore.
 * 
 * This function takes a semaphore. If the semaphore's value is zero,
 * it blocks the current task and switches to another task in the pending
 * task queue. It decrements the value of the semaphore afterwards.
 * 
 * @param lock Pointer to the semaphore to be taken.
 */
void jock_os_takeSempahore(Semaphore* lock);

/**
 * @brief Gives a semaphore.
 * 
 * This function gives a semaphore. It increments the value of the semaphore,
 * and if there are pending tasks waiting on the semaphore, it moves the first
 * pending task to the ready state and moves it from the pending task queue to
 * the ready task queue.
 * 
 * @param lock Pointer to the semaphore to be given.
 */
void jock_os_giveSempahore(Semaphore* lock);

/**
 * @brief Suspends the current running task for a fixed amount of time.
 * 
 * @param delay_ms (milliseconds)
 */
void jock_os_sleep(uint32_t delay_ms);

#endif // _SEMAPHORE_H_
