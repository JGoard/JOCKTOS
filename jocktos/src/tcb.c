#include "tcb.h"
#include <stdlib.h>

T_TCBError JOCKTOS_TCBError = {0, 0, 0, 0, 0};

void insertTCB(volatile T_TaskControlBlock** head, volatile T_TaskControlBlock* tcb) {
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

void removeTCB(volatile T_TaskControlBlock** head, volatile T_TaskControlBlock* tcb) {
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
        volatile T_TaskControlBlock* current = *head;
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

void updateTCB(volatile T_TaskControlBlock** head, volatile T_TaskControlBlock* tcb, uint8_t priority) {
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
    if (tcb->u8Priority == priority) return; // NOP if priority doesn't change
    removeTCB(head, tcb);          // Remove the node from the list
    tcb->u8Priority = priority; // Update the priority of the node
    insertTCB(head, tcb);          // Reinsert the node into the list at its new position
}

void moveTCB(volatile T_TaskControlBlock** currentHead, volatile T_TaskControlBlock** newHead, volatile T_TaskControlBlock* tcb) {
    // catch and log invalid linked list head
    if (*currentHead == NULL || *newHead == NULL) {
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
