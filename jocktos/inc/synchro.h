#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <stdint.h>
#include "tcb.h"

typedef struct {
    uint16_t value_;                      ///< [INTERNAL] current 
    uint16_t count;                       ///< Queue size for lock instance
    volatile T_TaskControlBlock *pendingTCBQueue_; ///< [INTERNAL] Linked list of tasks awaiting lock
    uint8_t ownersPriority_;              ///< [INTERNAL] TODO: prevent priority inversion
} T_Semaphore;

#define CREATE_SEMAPHORE(...) \
{   /* ---Internal Data---*/  \
    .value_           = 1,    \
    .count            = 2,    \
    .pendingTCBQueue_ = NULL, \
    .ownersPriority_  = 0,    \
     __VA_ARGS__              \
}

void takeSemaphore(T_Semaphore* lock);

void giveSemaphore(T_Semaphore* lock);

#endif // __SEMAPHORE_H__