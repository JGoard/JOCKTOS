/**
 * @file tcb.h
 * 
 * A @ref TaskControlBlock "Task Control Block" (TCB) is a data structure that stores information about an indiviual task in JOCKTOS.
 * This information includes the task's priority, name, function to run, function argument, the tasks state, and stack management information. 
 * The TCB is a fundamental building block of JOCKTOS and is used to manage the execution of tasks in the system. 
 * The JOCKTOS scheduler uses the TCBs to determine which task to run next.
 * The TCBs are created and managed by the application at runtime, allowing for a high degree of flexibility in the design of the system. 
 * They can be added to or removed from the list of tasks at any time, allowing for dynamic changes to the system's behavior.
 *
 * @section structure Linked List Structure
 * TCBs are organized in singly linked lists, sorted by their priority, with the highest priority value at the head of the list.
 * If multiple TCBs have the same priority value, they are inserted in a First-In-First-Out ordering amoung those with equal priority values.
 * Functions are provided to handle insertion, removal, reordering (updating a TCB's priority) and migrating between different lists while keeping the linked list sorted.
 * These functions do not throw errors, they log errors and exit with a NOP.
 * The errors are logged to a global error data structure @ref TCBError "TCBError"  which tracks errors as they occur.
 * 
 * @section methods Task Control Block Methods
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
 * @brief Default values for a task control block.
 *
 * @details
 * This macro creates a TaskControlBlock with the following default values:
 *  - priority: 0 (lowest priority)
 *  - name: empty string
 *  - delay_ms: 0 (no delay_ms)
 *  - stack_size_bytes: 0 (no stack allocated)
 *  - task_handle: NULL (no function to run)
 *  - task_arg: NULL (no argument to pass to function)
 *  - state: BLOCKED (task is not running and is awaiting a resource)
 *  - stack_pointer: NULL (no stack allocated)
 *  - next: NULL (not in any list)
 *
 * This macro is intended to be used for declaring a TaskControlBlock with
 * default values, with custom the values as needed. For example:
 *
 * @code
 * TaskControlBlock myTask = TASKCONTROLBLOCK_DEF(
 *         .name = "myTask",
 *         .task_handle = myTaskFunction,
 *         .stack_size_bytes = 1024);
 * @endcode
 *
 * @param ... varargs to override default values
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
 * @brief Enumeration of possible task states.
 * 
 * @details
 * Defines the possible states of a task within JOCKTOS. 
 * Each state represents a different phase in the lifecycle of a task, 
 * providing the scheduler and other system components with essential information about the task's current status.
 * 
 */
typedef enum {
    RUNNING    = 0,    ///< The task is currently executing on the CPU with priority over others.
    READY      = 1,    ///< The task is ready and waiting for execution by the scheduler.
    BLOCKED    = 2,    ///< The task is waiting for a resource (e.g., semaphore, mutex) to become available.
    SUSPENDED  = 3     ///< The task is temporarily inactive and can be reactivated by an event.
} TaskState;

/**
 * @brief Collection of counters for various error conditions encountered by the Task Control Block (TCB).
 *
 * @details
 * This structure contains variables that keep track of the number of times each error condition has occurred.
 */
typedef struct {
    volatile uint16_t invalid_task_handle;   ///< TaskControlBlock initialized with a task handle
    volatile uint16_t failed_to_allocate;    ///< TaskControlBlock initialized failed to allocate the requested task stack size
    volatile uint16_t invalid_list_head;     ///< Attempted to modify a list with a void list head
    volatile uint16_t invalid_tcb;           ///< Void TaskControlBlock pointer used in a list
    volatile uint16_t invalid_list_element;  ///< Provided a TCB that is not in the list
} TCBError;

/**
 * @brief Task Control Block (TCB).
 *
 * @details
 * The TaskControlBlock is a structure that represents a task in the system.
 * It contains information about the task's priority, name, stack size, and
 * other relevant data.
 *
 * See \ref  TASKCONTROLBLOCK_DEF "TASKCONTROLBLOCK_DEF(...)" for information on the default values.
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
 * \param head Pointer to the head of the linked list.
 * \param tcb Pointer to the task control block to be inserted.
 */
void insertTCB(volatile TaskControlBlock** head, volatile TaskControlBlock* tcb);

/**
 * \brief Remove a task control block from a linked list.
 *
 * \param head Pointer to the pointer to the head of the linked list.
 * \param tcb Pointer to the task control block to be removed.
 */
void removeTCB(volatile TaskControlBlock** head, volatile TaskControlBlock* tcb);

/**
 * \brief Update the priority of a task control block in a linked list.
 *
 * \param head Pointer to the pointer to the head of the linked list.
 * \param tcb Pointer to the task control block to be updated.
 * \param priority The new priority for the task control block.
 */
void updateTCB(volatile TaskControlBlock** head, volatile TaskControlBlock* tcb, uint8_t priority);

/**
 * \brief Move a task control block from one linked list to another.
 *
 *
 * \param source Pointer to the pointer to the head of the current linked list.
 * \param tcb Pointer to the task control block to be moved.
 * \param destination Pointer to the pointer to the head of the new linked list.
 */
void moveTCB(volatile TaskControlBlock** source, volatile TaskControlBlock* tcb, volatile TaskControlBlock** destination);

#endif // _TCB_H_
