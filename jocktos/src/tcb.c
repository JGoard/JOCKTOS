/**
* \brief This module contains the logic for a single linked list of
* Task Control Blocks
*/
/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "tcb.h"
// Middleware
// Bios
// Standard C
#include <stdlib.h>

/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Local Globals (not for libraries with application instantiation) ---- */

TCBError JOCKTOS_TCBError = {0};

/* -- Private Function Declarations --------------------------------------- */

/* -- Public Functions----------------------------------------------------- */
void insertTCB(volatile TaskControlBlock** head, volatile TaskControlBlock* tcb) {
    if (head == NULL) {
        JOCKTOS_TCBError.invalidListHead++;
        return;
    }
    // if tcb is the first entry or the highest priority, updated the head
    if (*head == NULL || tcb->priority > (*head)->priority) {
        tcb->TCBNext = *head;
        *head = tcb;
    } else {
        // otherwise, iterate across the linked list and insert tcb wrt its priority
        volatile TaskControlBlock* current = *head;
        while (current->TCBNext != NULL && current->TCBNext->priority >= tcb->priority) {
            current = current->TCBNext;
        }
        tcb->TCBNext = current->TCBNext;
        current->TCBNext = tcb;
    }
}

void removeTCB(volatile TaskControlBlock** head, volatile TaskControlBlock* tcb) {
    // catch and log invalid linked list head
    if (*head == NULL) {
        JOCKTOS_TCBError.invalidListHead++;
        return;
    }
    // catch and log invalid control block reference
    if (tcb == NULL) {
        JOCKTOS_TCBError.invalidTCB++;
        return;
    }

    if (*head == tcb) {
        *head = tcb->TCBNext;
    } else {
        volatile TaskControlBlock* current = *head;
        while (current->TCBNext != NULL && current->TCBNext != tcb) {
            current = current->TCBNext;
        }
        if (current->TCBNext == tcb) {
            current->TCBNext = tcb->TCBNext;
        } else {
            // log attempted removal of non-existant TCB
            JOCKTOS_TCBError.invalidListElement++;
        }
    }
}

void updateTCB(volatile TaskControlBlock** head, volatile TaskControlBlock* tcb, uint8_t priority) {
    // catch and log invalid linked list head
    if (*head == NULL) {
        JOCKTOS_TCBError.invalidListHead++;
        return;
    }
    // catch and log invalid control block reference
    if (tcb == NULL) {
        JOCKTOS_TCBError.invalidTCB++;
        return;
    }
    if (tcb->priority == priority) return; // NOP if priority doesn't change
    removeTCB(head, tcb);          // Remove the node from the list
    tcb->priority = priority; // Update the priority of the node
    insertTCB(head, tcb);          // Reinsert the node into the list at its new position
}

void moveTCB(volatile TaskControlBlock** currentHead, volatile TaskControlBlock** newHead, volatile TaskControlBlock* tcb) {
    // catch and log invalid linked list head
    if (currentHead == NULL || newHead == NULL) {
        JOCKTOS_TCBError.invalidListHead++;
        return;
    }
    // catch and log invalid control block reference
    if (tcb == NULL) {
        JOCKTOS_TCBError.invalidTCB++;
        return;
    }
    removeTCB(currentHead, tcb);
    insertTCB(newHead, tcb);
}

/* -- Private Functions --------------------------------------------------- */
