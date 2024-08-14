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
void jock_takeSemaphore(Semaphore* lock) {
    __asm volatile ("cpsid i" : : : "memory");
    if (!lock->value_) jock_os_switchRunningTask(&lock->pending_queue_);
    JOCKTOSScheduler.running->eState = eBLOCKED;
    __asm volatile ("cpsie i" : : : "memory");

    __asm volatile ("cpsid i" : : : "memory");
    lock->value_--;
    __asm volatile ("cpsie i" : : : "memory");
}

void jock_giveSemaphore(Semaphore* lock) {
    __asm volatile ("cpsid i" : : : "memory");  
    lock->value_ = (lock->value_ + 1) % lock->count;
    if (lock->pending_queue_) {
        lock->pending_queue_->state = READY;
        moveTCB(&lock->pending_queue_, lock->pending_queue_, &JOCKTOSScheduler.ready);
    }
    __asm volatile ("cpsie i" : : : "memory");
    return;
}

void jock_sleep(uint32_t delay_ms) {
    __asm volatile ("cpsid i" : : : "memory");
    JOCKTOSScheduler.running->delay_ms = jock_jock_os_currentTime() + delay_ms;
    JOCKTOSScheduler.running->state = SUSPENDED;
    jock_os_switchRunningTask(&JOCKTOSScheduler.suspended);
    __asm volatile ("cpsie i" : : : "memory");
    return;

}

/* -- Private Functions --------------------------------------------------- */
