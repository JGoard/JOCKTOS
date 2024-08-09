/**
* \brief This header is to act as companion header for tcb.c
*/
#ifndef _TCB_H_
#define _TCB_H_
/* -- Includes ------------------------------------------------------------ */
// Jocktos
// Middleware
// Bios
// Standard C
#include <stdint.h>
#include <stddef.h>

/* -- Defines ------------------------------------------------------------- */

/**
 * @brief default task control block
 */
#define TASKCONTROLBLOCK_DEF(...)   \
{   /* ---- Configured ---*/        \
    .priority = 0,                  \
    .name     = "\0",               \
    .delay_ms = 0,                  \
    .stack_size_bytes = 0,          \
    /* ---- Input Data ---*/        \
    .task_handle = NULL,            \
    .task_arg    = NULL,            \
    /* ---- Output Data---*/        \
    .state = BLOCKED,               \
    /* ---- Internal Data-*/        \
    .stack_pointer = NULL,          \
    .next          = NULL,          \
     __VA_ARGS__                    \
}

/* -- Types --------------------------------------------------------------- */

/** 
 * @brief Enumeration of possible task states (stale feature)
 */
typedef enum {
    RUNNING    = 0,    /**< Currently active task. */
    READY      = 1,    /**< In the queue and ready to run. */
    BLOCKED    = 2,    /**< Awaiting a resource. */
    SUSPENDED  = 3    /**< Delayed or intentionally released. */
} TaskState;

/** 
 * @brief colelction of potential error counters
 */
typedef struct {
    volatile uint16_t invalid_task_handle;   ///< TaskControlBlock initialized with a task handle
    volatile uint16_t failed_to_allocate;    ///< TaskControlBlock initialized failed to allocate the requested task stack size
    volatile uint16_t invalid_list_head;     ///< Attempted to modify a list with a void list head
    volatile uint16_t invalid_tcb;           ///< Void TaskControlBlock pointer used in a list
    volatile uint16_t invalid_list_element;  ///< Provided a TCB that is not in the list
} TCBError;

/** 
 * @brief Task function handle
 */
typedef void (*FunctionHandle)(void*);

/** 
 * @brief Cortex-M4 Context Control Block
 */
typedef struct TaskControlBlock {
    volatile double  stack_usage;               ///< Percentage of stack used as of last preemption
    volatile uint8_t priority;                  ///< The priority of the task
    char*            name;                      ///< Name of the task
    uintptr_t        stack_size_bytes;          ///< Configured task stack size
    uint32_t         delay_ms;                  ///< Delay in ms on 
    void             (*task_handle)(void*);     ///< Main function handle for task
    void*            task_arg;                  ///< Argument to be passed into the task function
    volatile TaskState state;                   ///< Defines current task state
    uintptr_t*          stack_overflow;         ///< Lowest accessible address for this tasks stack pointer
    volatile uintptr_t* stack_pointer;          ///< Hold's the current task stack pointer
    volatile struct TaskControlBlock* next;     ///< Next item for singly linked list
} TaskControlBlock;

/* -- Externs (avoid these for library functions) ------------------------- */

/* -- Function Declarations ----------------------------------------------- */

/**
 * \brief Insert a task control block into a linked list.
 *
 * This function inserts a task control block into a linked list in descending order of priority.
 *
 * \param head Pointer to the head of the linked list.
 * \param tcb Pointer to the task control block to be inserted.
 */
void insertTCB(volatile TaskControlBlock** head, volatile TaskControlBlock* tcb);

/**
 * \brief Remove a task control block from a linked list.
 *
 * This function removes a task control block from a linked list. If the task control block is found,
 * it is removed from the list; otherwise, an error is logged.
 *
 * \param head Pointer to the pointer to the head of the linked list.
 * \param tcb Pointer to the task control block to be removed.
 */
void removeTCB(volatile TaskControlBlock** head, volatile TaskControlBlock* tcb);

/**
 * \brief Update the priority of a task control block in a linked list.
 *
 * This function updates the priority of a task control block in a linked list. It first removes the
 * task control block from the list, then updates its priority, and finally reinserts it into the list
 * at its new position based on the updated priority.
 *
 * \param head Pointer to the pointer to the head of the linked list.
 * \param tcb Pointer to the task control block to be updated.
 * \param priority The new priority for the task control block.
 */
void updateTCB(volatile TaskControlBlock** head, volatile TaskControlBlock* tcb, uint8_t priority);

/**
 * \brief Move a task control block from one linked list to another.
 *
 * This function moves a task control block from one linked list to another. It first removes the task
 * control block from the current linked list, and then inserts it into the new linked list.
 *
 * \param source Pointer to the pointer to the head of the current linked list.
 * \param tcb Pointer to the task control block to be moved.
 * \param destination Pointer to the pointer to the head of the new linked list.
 */
void moveTCB(volatile TaskControlBlock** source, volatile TaskControlBlock* tcb, volatile TaskControlBlock** destination);

#endif // _TCB_H_
