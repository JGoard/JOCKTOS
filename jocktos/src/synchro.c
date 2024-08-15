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
    CRITICAL_SECTION(
        if (!lock->value_) {
            JOCKTOSScheduler.running->state = BLOCKED;
            switchRunningTask(&lock->pending_queue_);
        }
    );

    CRITICAL_SECTION(
        lock->value_--;
    );
}

void jock_giveSemaphore(Semaphore* lock) {
    CRITICAL_SECTION( 
        lock->value_ = (lock->value_ + 1) % lock->count;
        if (lock->pending_queue_) {
            lock->pending_queue_->state = READY;
            moveTCB(&lock->pending_queue_, lock->pending_queue_, &JOCKTOSScheduler.ready);
        }
    );
    return;
}

void jock_sleep(uint32_t delay_ms) {
    CRITICAL_SECTION(
        JOCKTOSScheduler.running->delay_ms = jock_currentTime() + delay_ms;
        JOCKTOSScheduler.running->state = SUSPENDED;
        switchRunningTask(&JOCKTOSScheduler.suspended);
    );
    return;

}

/* -- Private Functions --------------------------------------------------- */
