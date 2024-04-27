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
#define T_SEMAPHORE_DEF(...) \
{   /* ---Internal Data---*/  \
    .value_           = 1,    \
    .count            = 2,    \
    .pendingTCBQueue_ = NULL, \
    .ownersPriority_  = 0,    \
     __VA_ARGS__              \
}

/* -- Types --------------------------------------------------------------- */

/**
 *  @brief Represents a semaphore for controlling access to a shared resource. 
 */
typedef struct {
    uint16_t value_;                                ///< [INTERNAL] current value
    uint16_t count;                                 ///< Queue size for lock instance
    volatile T_TaskControlBlock *pendingTCBQueue_;  ///< [INTERNAL] Linked list of tasks awaiting lock
    uint8_t ownersPriority_;                        ///< [INTERNAL] TODO: prevent priority inversion
} T_Semaphore;

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
void takeSemaphore(T_Semaphore* lock);

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
void giveSemaphore(T_Semaphore* lock);

/**
 * @brief Suspends the current running task for a fixed amount of time.
 * 
 * @param delay (milliseconds)
 */
void sleep(uint32_t delay);

#endif // _SEMAPHORE_H_
