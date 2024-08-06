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

extern Scheduler JOCKTOSScheduler;

/* -- Private Function Declarations --------------------------------------- */

/* -- Public Functions----------------------------------------------------- */
void takeSemaphore(Semaphore* lock) {
    __asm volatile ("cpsid i" : : : "memory");
    if (!lock->value_) switchRunningTask(&lock->pendingTCBQueue_);
    JOCKTOSScheduler.running->state = BLOCKED;
    __asm volatile ("cpsie i" : : : "memory");

    __asm volatile ("cpsid i" : : : "memory");
    lock->value_--;
    __asm volatile ("cpsie i" : : : "memory");
}

void giveSemaphore(Semaphore* lock) {
    __asm volatile ("cpsid i" : : : "memory");  
    lock->value_ = (lock->value_ + 1) % lock->count;
    if (lock->pendingTCBQueue_) {
        lock->pendingTCBQueue_->state = READY;
        moveTCB(&lock->pendingTCBQueue_,
                &JOCKTOSScheduler.ready,
                lock->pendingTCBQueue_);
    }
    __asm volatile ("cpsie i" : : : "memory");
    return;
}

void sleep(uint32_t delay) {
    __asm volatile ("cpsid i" : : : "memory");
    JOCKTOSScheduler.running->delay = currentTime() + delay;
    JOCKTOSScheduler.running->state = SUSPENDED;
    switchRunningTask(&JOCKTOSScheduler.suspended);
    __asm volatile ("cpsie i" : : : "memory");
    return;

}

/* -- Private Functions --------------------------------------------------- */
