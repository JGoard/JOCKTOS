/**
* \brief This module is to...
*/

/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "os.h"
// Middleware
// Bios
// Standard C
#include <stdlib.h>
/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Local Globals (not for libraries with application instantiation) ---- */

T_Scheduler JOCKTOSScheduler;
/* -- Private Function Declarations --------------------------------------- */
_Bool createTask(T_TaskControlBlock* tcb) {
    if (!tcb->taskFunct) return 0;
    tcb->u32TaskStackPointer = (uint32_t*) malloc(tcb->u32StackSize_By * sizeof(uint32_t));
    if (!tcb->u32TaskStackPointer) return 0;
    insert(&JOCKTOSScheduler.ready, tcb);
    return 1;
}
/* -- Public Functions----------------------------------------------------- */

void insert(T_TaskControlBlock** head, T_TaskControlBlock* tcb) {

    if (*head == NULL || tcb->u8Priority >= (*head)->u8Priority) {
        tcb->TCBNext = *head;
        *head = tcb;
    } else {
        T_TaskControlBlock* current = *head;
        while (current->TCBNext != NULL && current->TCBNext->u8Priority > tcb->u8Priority) {
            current = current->TCBNext;
        }
        tcb->TCBNext = current->TCBNext;
        current->TCBNext = tcb;
    }
}

void remove(T_TaskControlBlock** head, T_TaskControlBlock* tcb) {
    // status.removeFailed = false;

    if (*head == NULL || tcb == NULL) {
        // status.removeFailed = true;
        return;
    }

    if (*head == tcb) {
        *head = tcb->TCBNext;
    } else {
        T_TaskControlBlock* current = *head;
        while (current->TCBNext != NULL && current->TCBNext != tcb) {
            current = current->TCBNext;
        }
        if (current->TCBNext == tcb) {
            current->TCBNext = tcb->TCBNext;
        } else {
            // status.removeFailed = true;
        }
    }
}

void update(T_TaskControlBlock** head, T_TaskControlBlock* tcb, uint8_t priority) {
    // status.updateFailed = false;

    if (*head == NULL || tcb == NULL) {
        // status.updateFailed = true;
        return;
    }

    // Remove the node from the list
    remove(head, tcb);

    // Update the priority of the node
    tcb->u8Priority = priority;

    // Reinsert the node into the list at its new position
    insert(head, tcb);
}