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
void jock_synchro_takeSempahore(Semaphore* lock) {
    uint32_t primask = 0;
    CRITICAL_SECTION(primask,{
        if (!lock->value_) {
            JOCKTOSScheduler.running->state = BLOCKED;
            jock_os_switchRunningTask(&lock->pending_queue_);
        }
    }
    );

    CRITICAL_SECTION(primask,{
        lock->value_--;
    }
    );
}

void jock_synchro_giveSempahore(Semaphore* lock) {
    uint32_t primask = 0;
    CRITICAL_SECTION(primask,{
        lock->value_ = (lock->value_ + 1) % lock->count;
        if (lock->pending_queue_) {
            lock->pending_queue_->state = READY;
            moveTCB(&lock->pending_queue_, lock->pending_queue_, &JOCKTOSScheduler.ready);
        }
    }
    );
    return;
}

void jock_synchro_sleep(uint32_t delay_ms) {
    uint32_t primask = 0;
    CRITICAL_SECTION(primask,
        JOCKTOSScheduler.running->delay_ms = jock_os_currentTime() + delay_ms;
        JOCKTOSScheduler.running->state = SUSPENDED;
        jock_os_switchRunningTask(&JOCKTOSScheduler.suspended);
    );
    return;

}

/* -- Private Functions --------------------------------------------------- */
