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

/**
 * @details
 * This function inserts a @ref TaskControlBlock "TaskControlBlock" into a linked list. 
 * This linked list is sorted based on its priority value, with the highest priority Task Control Block at the head of the list. 
 * If the linked list is empty the TaskControlBlock is inserted at the head of the list. 
 * Otherwise, the function iterates across the linked list and inserts the TaskControlBlock in its correct position based on its priority value. 
 * If multiple TaskControlBlocks have the same priority value they are inserted in a First-In-First-Out ordering amoung those with equal priority values.
 *
 * @warning 
 *  - If the linked list head is NULL, the function logs an invalid_list_head error and returns with no action.
 *  - If the TaskControlBlock pointer is NULL, the function logs an invalid_tcb error and returns with no action.
 */
void _insert_tcb(volatile TaskControlBlock** head, volatile TaskControlBlock* tcb) {
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

/**
 * @details
 * This function removes a @ref TaskControlBlock "TaskControlBlock" from a linked list. 
 * If the TaskControlBlock is found, it is removed from the list; otherwise, an error is logged. 
 * 
 * @warning 
 *  - If the TaskControlBlock is not found in the linked list, the function logs an invalid_list_element error and returns with no action.
 *  - If the linked list head is NULL, the function logs an invalid_list_head error and returns with no action.
 *  - If the TaskControlBlock pointer is NULL, the function logs an invalid_tcb error and returns with no action.
 */
void _remove_tcb(volatile TaskControlBlock** head, volatile TaskControlBlock* tcb) {
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

/**
 * @details
 * This function updates the priority of a @ref TaskControlBlock "TaskControlBlock" in a linked list. 
 * It first removes the TaskControlBlock from the list, then updates its priority, 
 * and finally reinserts it into the list at its new position based on the updated priority.
 * 
 * @warning 
 *  - If the linked list head is NULL, the function logs an invalid_list_head error and returns with no action.
 *  - If the TaskControlBlock pointer is NULL, the function logs an invalid_tcb error and returns with no action.
 */
void _update_tcb(volatile TaskControlBlock** head, volatile TaskControlBlock* tcb, uint8_t priority) {
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
    if (tcb->priority == priority) return;
    _remove_tcb(head, tcb);
    tcb->priority = priority;
    _insert_tcb(head, tcb);
}

/**
 * @details
 * This function moves a @ref TaskControlBlock "TaskControlBlock" from one linked list to another. 
 * It first removes the TaskControlBlock from the current linked list, and then inserts it into the new linked list.
 * 
 * @warning 
 *  - If the linked list head is NULL, the function logs an invalid_list_head error and returns with no action.
 *  - If the TaskControlBlock pointer is NULL, the function logs an invalid_tcb error and returns with no action.
 */
void _move_tcb(volatile TaskControlBlock** source, volatile TaskControlBlock* tcb, volatile TaskControlBlock** destination) {
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
    _remove_tcb(source, tcb);
    _insert_tcb(destination, tcb);
}

/* -- Private Functions --------------------------------------------------- */
