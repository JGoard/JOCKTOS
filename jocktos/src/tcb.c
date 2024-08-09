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
        JOCKTOS_TCBError.invalid_list_head++;
        return;
    }
    if (tcb == NULL) {
        JOCKTOS_TCBError.invalid_tcb++;
        return;
    }
    // if tcb is the first entry or the highest priority, updated the head
    if (*head == NULL || tcb->priority > (*head)->priority) {
        tcb->next = *head;
        *head = tcb;
    } else {
        // otherwise, iterate across the linked list and insert tcb wrt its priority
        volatile TaskControlBlock* current = *head;
        while (current->next != NULL && current->next->priority >= tcb->priority) {
            current = current->next;
        }
        tcb->next = current->next;
        current->next = tcb;
    }
}

void removeTCB(volatile TaskControlBlock** head, volatile TaskControlBlock* tcb) {
    // catch and log invalid linked list head
    if (head == NULL || *head == NULL) {
        JOCKTOS_TCBError.invalid_list_head++;
        return;
    }
    // catch and log invalid control block reference
    if (tcb == NULL) {
        JOCKTOS_TCBError.invalid_tcb++;
        return;
    }

    if (*head == tcb) {
        *head = tcb->next;
    } else {
        volatile TaskControlBlock* current = *head;
        while (current->next != NULL && current->next != tcb) {
            current = current->next;
        }
        if (current->next == tcb) {
            current->next = tcb->next;
        } else {
            // log attempted removal of non-existant TCB
            JOCKTOS_TCBError.invalid_list_element++;
        }
    }
}

void updateTCB(volatile TaskControlBlock** head, volatile TaskControlBlock* tcb, uint8_t priority) {
    // catch and log invalid linked list head
    if (head == NULL ||*head == NULL) {
        JOCKTOS_TCBError.invalid_list_head++;
        return;
    }
    // catch and log invalid control block reference
    if (tcb == NULL) {
        JOCKTOS_TCBError.invalid_tcb++;
        return;
    }
    if (tcb->priority == priority) return; // NOP if priority doesn't change
    removeTCB(head, tcb);          // Remove the node from the list
    tcb->priority = priority; // Update the priority of the node
    insertTCB(head, tcb);          // Reinsert the node into the list at its new position
}

void moveTCB(volatile TaskControlBlock** source, volatile TaskControlBlock* tcb, volatile TaskControlBlock** destination) {
    // catch and log invalid linked list head
    if (source == NULL || destination == NULL) {
        JOCKTOS_TCBError.invalid_list_head++;
        return;
    }
    // catch and log invalid control block reference
    if (tcb == NULL) {
        JOCKTOS_TCBError.invalid_tcb++;
        return;
    }
    removeTCB(source, tcb);
    insertTCB(destination, tcb);
}

/* -- Private Functions --------------------------------------------------- */
