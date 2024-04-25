/**
* \brief This module defines the JOCKTOS synchronization mechanisms
*/
/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "synchro.h"
#include "os.h"
// Middleware
// Bios
// Standard C
#include <stdint.h>

/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Local Globals (not for libraries with application instantiation) ---- */

extern T_Scheduler JOCKTOSScheduler;

/* -- Private Function Declarations --------------------------------------- */

void takeSemaphore(T_Semaphore* lock) {
    __asm volatile ("cpsid i" : : : "memory");
    if (!lock->value_) switchRunningTask(&lock->pendingTCBQueue_);
    JOCKTOSScheduler.running->eState = eBLOCKED;
    __asm volatile ("cpsie i" : : : "memory");

    __asm volatile ("cpsid i" : : : "memory");
    lock->value_--;
    __asm volatile ("cpsie i" : : : "memory");
}

void giveSemaphore(T_Semaphore* lock) {
    __asm volatile ("cpsid i" : : : "memory");  
    lock->value_ = (lock->value_ + 1) % lock->count;
    if (lock->pendingTCBQueue_) {
        lock->pendingTCBQueue_->eState = eREADY;
        moveTCB(&lock->pendingTCBQueue_,
                &JOCKTOSScheduler.ready,
                lock->pendingTCBQueue_);
    }
    __asm volatile ("cpsie i" : : : "memory");
    return;
}

/* -- Public Functions----------------------------------------------------- */
