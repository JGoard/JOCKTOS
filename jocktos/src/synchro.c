#include "synchro.h"
#include "os.h"
#include <stdint.h>
uint32_t semaphore_tick = 0;
extern T_Scheduler JOCKTOSScheduler;
void takeSemaphore(T_Semaphore* lock) {
    CRITICAL_SECTION(
        if (!lock->value_) switchRunningTask(&lock->pendingTCBQueue_);
        JOCKTOSScheduler.running->eState = eBLOCKED;
    );
    CRITICAL_SECTION(
        lock->value_--;
        semaphore_tick++;

    ); 
}

void giveSemaphore(T_Semaphore* lock) {
    CRITICAL_SECTION(   
        lock->value_ = (lock->value_ + 1) % lock->count;
        if (lock->pendingTCBQueue_) {
            lock->pendingTCBQueue_->eState = eREADY;
            moveTCB(                        \
                &lock->pendingTCBQueue_,    \
                &JOCKTOSScheduler.ready,    \
                lock->pendingTCBQueue_);
        }
    );
    return;
}
