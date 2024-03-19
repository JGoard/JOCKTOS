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
T_OSError JOCKTOSError = {0, 0, 0};
uint32_t* psp_test = NULL;
uint32_t* msp_test = NULL;
/* -- Private Function Declarations --------------------------------------- */
/**    
 * Exception frame without floating-point storage 
 * 
 *        ...      <--\
 *  ---------------    |--- Pre-IRQ top of stack
 *     {aligner}   <--/
 *  ---------------
 *       xPSR 
 *  ---------------
 *        PC
 *  ---------------
 *        LR
 *  ---------------
 *        R12
 *  ---------------
 *        R3
 *  ---------------
 *        R2
 *  ---------------
 *        R1
 *  ---------------
 *        R0
 *  ---------------  
 *       ...      <----- IRQ top of stack
*/
/**
* \brief Place Holder OS tick handler
*
* Copies the Main Stack Pointer (MSP) and Process Stack Pointer (PSP) into globals
*
* \return
*/
void SysTick_Handler(void) {

    __asm volatile ("MRS %0, MSP" : "=r" (msp_test) :: "memory");
    __asm volatile ("MRS %0, PSP" : "=r" (psp_test) :: "memory");

}

void createTask(T_TaskControlBlock* tcb) {
    if (!tcb->taskFunct) {
        JOCKTOSError.invalidTaskHandle++;
        return;
    }
    uint32_t* taskStack =(uint32_t*) malloc(tcb->u32StackSize_By * sizeof(uint32_t));
    if (!taskStack) {
        JOCKTOSError.failedToAllocate++;
        return;
        }
    tcb->u32TaskStackOverflow = taskStack;
    tcb->u32TaskStackPointer = taskStack + tcb->u32StackSize_By; // possibly needs offset from the very bottom
    insert(&JOCKTOSScheduler.ready, tcb);
}

void switchTask(void) {
    // move the current running task into READY if it exists (not NULL)
    if (JOCKTOSScheduler.running) {
        JOCKTOSScheduler.running->eState = eREADY;
        insert(&JOCKTOSScheduler.ready, JOCKTOSScheduler.running);
    }
    // Pop task of the top of READY linked-list and move into running
    JOCKTOSScheduler.ready->eState = eRUNNING;
    JOCKTOSScheduler.running = JOCKTOSScheduler.ready;
    remove(&JOCKTOSScheduler.ready, JOCKTOSScheduler.running);
}

void insert(volatile T_TaskControlBlock** head, volatile T_TaskControlBlock* tcb) {
    // if tcb is the first entry or the highest priority, updated the head
    if (*head == NULL || tcb->u8Priority > (*head)->u8Priority) {
        tcb->TCBNext = *head;
        *head = tcb;
    } else {
        // otherwise, iterate across the linked list and insert tcb wrt its priority
        volatile T_TaskControlBlock* current = *head;
        while (current->TCBNext != NULL && current->TCBNext->u8Priority >= tcb->u8Priority) {
            current = current->TCBNext;
        }
        tcb->TCBNext = current->TCBNext;
        current->TCBNext = tcb;
    }
}

void remove(volatile T_TaskControlBlock** head, volatile T_TaskControlBlock* tcb) {
    // catch and log invalid linked list head
    if (*head == NULL) {
        JOCKTOSError.invalidListHead++;
        return;
    }
    // catch and log invalid control block reference
    if (tcb == NULL) {
        JOCKTOSError.invalidTCB++;
        return;
    }

    if (*head == tcb) {
        *head = tcb->TCBNext;
    } else {
        volatile T_TaskControlBlock* current = *head;
        while (current->TCBNext != NULL && current->TCBNext != tcb) {
            current = current->TCBNext;
        }
        if (current->TCBNext == tcb) {
            current->TCBNext = tcb->TCBNext;
        } else {
            // log attempted removal of non-existant TCB
            JOCKTOSError.invalidListElement++;
        }
    }
}

void update(volatile T_TaskControlBlock** head, volatile T_TaskControlBlock* tcb, uint8_t priority) {
    // catch and log invalid linked list head
    if (*head == NULL) {
        JOCKTOSError.invalidListHead++;
        return;
    }
    // catch and log invalid control block reference
    if (tcb == NULL) {
        JOCKTOSError.invalidTCB++;
        return;
    }
    remove(head, tcb);          // Remove the node from the list
    tcb->u8Priority = priority; // Update the priority of the node
    insert(head, tcb);          // Reinsert the node into the list at its new position
}

void move(volatile T_TaskControlBlock** currentHead, volatile T_TaskControlBlock** newHead, volatile T_TaskControlBlock* tcb) {
    // catch and log invalid linked list head
    if (*currentHead == NULL || *newHead == NULL) {
        JOCKTOSError.invalidListHead++;
        return;
    }
    // catch and log invalid control block reference
    if (tcb == NULL) {
        JOCKTOSError.invalidTCB++;
        return;
    }
    remove(currentHead, tcb);
    insert(newHead, tcb);
}

/* -- Public Functions----------------------------------------------------- */
