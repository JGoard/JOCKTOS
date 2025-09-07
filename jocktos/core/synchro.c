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
void jock_synchro_take_sempahore(Semaphore* lock) {
    uint32_t primask = 0;
    primask = jock_os_enter_critical_section();
    if (!lock->value_) {
        JOCKTOSScheduler.running->state = BLOCKED;
        jock_os_switch_running_task(&lock->pending_queue_);
    }
    jock_os_leave_critical_section(primask);

    primask = jock_os_enter_critical_section();
        lock->value_--;
    jock_os_leave_critical_section(primask);
    return;

}

void jock_synchro_give_sempahore(Semaphore* lock) {
    uint32_t primask = 0;
    primask = jock_os_enter_critical_section();
        lock->value_ = (lock->value_ + 1) % lock->count;
        if (lock->pending_queue_) {
            lock->pending_queue_->state = READY;
            _move_tcb(&lock->pending_queue_, lock->pending_queue_, &JOCKTOSScheduler.ready);
        }
    jock_os_leave_critical_section(primask);
    return;
}

void jock_synchro_sleep(uint32_t delay_ms) {
    uint32_t primask = 0;
    primask = jock_os_enter_critical_section();
    JOCKTOSScheduler.running->delay_ms = jock_os_get_time() + delay_ms;
    JOCKTOSScheduler.running->state = SUSPENDED;
    jock_os_switch_running_task(&JOCKTOSScheduler.suspended);
    jock_os_leave_critical_section(primask);

    return;

}

/* -- Private Functions --------------------------------------------------- */
