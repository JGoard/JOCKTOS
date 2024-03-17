/**
* \brief This module is the core JOCKTOS kernel functionality
*/
#ifndef _OS_H_
#define _OS_H_
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

#define T_TASKCONTROLBLOCK_DEF(...) \
{   /* ---- Configured ---*/        \
    .u8Priority      = 0,           \
    .u8Name          = 0,           \
    .u32Delay        = 0,           \
    .u32StackSize_By = 0,           \
    /* ---- Input Data ---*/        \
    .taskFunct       = NULL,        \
    .taskArg         = NULL,        \
    /* ---- Output Data---*/        \
    .eState          = eBLOCKED,    \
    /* ---- Internal Data-*/        \
    .u32TaskStackPointer = NULL,    \
    .TCBNext             = NULL,    \
     __VA_ARGS__                    \
}

#define CRITICAL_SECTION(arg)                      \
    do {                                           \
        __asm volatile ("cpsid i" : : : "memory"); \
        arg;                                       \
        __asm volatile ("cpsie i" : : : "memory"); \
    } while (0)

/* -- Types --------------------------------------------------------------- */

//=============================================================================================
/**
 * @brief Task State Machine Enumeration
 * 
 * @warning super janky image:
 * \code{.unparsed}
 *                                     -------------
 *                      /-->----->---->| SUSPENDED |<-----<-----<--\
 *                     /               -------------                \
 *                    /                    ^   v                     \
 *                   /          suspend(); |   | resume();            |
 *                  |                      ^   v                      ^
 *                  ^                    ---------   scheduler   -----------
 *                  | suspend();         | READY | >----->-----> | RUNNING |
 *                  ^                    |       | <-----<-----< |         |
 *                  |                    ---------               -----------
 *                   \                       ^                        v
 *                    \                      | event                  |
 *                     \                     ^                       /
 *                      \               -----------                 /
 *                       \<-----<-----<-| BLOCKED |<-----<-----<---/
 *                                      -----------
 * \endcode
 * 
 * @attention Wild inline LaTeX
 * @f[
 * \int_a^b f(x) dx = F(b) - F(a)
 * @f]
 * 
 */                   

typedef enum {
    eRUNNING    = 0,    /**< Currently active task. */
    eREADY      = 1,    /**< In the queue and ready to run. */
    eBLOCKED    = 2,    /**< Awaiting a resource. */
    eSUSPENDED  = 3,    /**< Delayed or intentionally released. */
    eTaskStateAmount
} E_TaskState;

//=============================================================================================

typedef void (*T_FunctionHandle)(uint32_t*);
typedef struct T_TaskControlBlock T_TaskControlBlock; ///< the compiler did not like `typedef struct X {...} X;`
/** 
 * @brief Cortex-M4 Context Control Block
 */
struct T_TaskControlBlock {
    /* ---- Configured ---*/
    volatile uint8_t    u8Priority;          ///<    The priority of the task
    char*               u8Name;              ///<    Name of the tast
    uint32_t            u32StackSize_By;     ///<    Configured task stack size
    uint32_t            u32Delay;            ///<    Delay in ms on 
    /* ---- Input Data ---*/
    T_FunctionHandle    taskFunct;           ///<    Main function handle for task
    void*               taskArg;             ///<    Argument to be passed into the task function
    /* ---- Output Data---*/
    volatile E_TaskState         eState;     ///<    Defines current task state
    /* ---s- Working Data--*/
    /* ---- Internal Data-*/
    uint32_t*                    u32TaskStackOverflow; ///<    lowest accessible address for this tasks stack pointer
    volatile uint32_t*           u32TaskStackPointer;  ///<    Hold's the current task stack pointer
    volatile uint32_t*           u32ProgramCounter;    ///<    Hold's the current tasks program counter
    volatile T_TaskControlBlock* TCBNext;              ///<    Next item for singly linked list
};

/** 
 * @brief Cortex-M4 Context Control Block
 */
typedef struct {
    volatile _Bool               active;    ///<    Initialization flag
    volatile T_TaskControlBlock* running;   ///<    Currently running task
    volatile T_TaskControlBlock* ready;     ///<    Singly linked list of tasks ready to run, in decending order of priority
    volatile T_TaskControlBlock* blocked;   ///<    Singly linked list of blocked tasks, in decending order of priority
    volatile T_TaskControlBlock* suspended; ///<    Singly linked list of suspended tasks, in decending order of priority
} T_Scheduler;

/** 
 * @brief initial Error struct 
 * TODO: custom messages, timestamps, severity etc.
 */
typedef struct {
    volatile uint16_t invalidTaskHandle;
    volatile uint16_t failedToAllocate;
    volatile uint16_t invalidListHead;
    volatile uint16_t invalidTCB;
    volatile uint16_t invalidListElement;
} T_OSError;

/* -- Externs (avoid these for library functions) ------------------------- */

/* -- Function Declarations ----------------------------------------------- */

/**
 * \brief Create a new task and add it to the scheduler.
 *
 * This function creates a new task by allocating memory for its stack, initializes the task stack pointer,
 * and inserts the task control block into the scheduler's ready queue.
 *
 * \param tcb Pointer to the task control block representing the new task.
 */
void createTask(T_TaskControlBlock* tcb);

/**
* \brief Switch the currently running task
*
* Updates the Schedulers linked lists and task states upon context switch 
* TODO: Only moves between running and ready, will need to move between suspended and blocked once 
* the resource locks are ready
*
* \return
*/
void switchTask(void);

/**
 * \brief Insert a task control block into a linked list.
 *
 * This function inserts a task control block into a linked list in descending order of priority.
 *
 * \param head Pointer to the head of the linked list.
 * \param tcb Pointer to the task control block to be inserted.
 */
void insert(volatile T_TaskControlBlock** head, volatile T_TaskControlBlock* tcb);

/**
 * \brief Remove a task control block from a linked list.
 *
 * This function removes a task control block from a linked list. If the task control block is found,
 * it is removed from the list; otherwise, an error is logged.
 *
 * \param head Pointer to the pointer to the head of the linked list.
 * \param tcb Pointer to the task control block to be removed.
 */
void remove(volatile T_TaskControlBlock** head, volatile T_TaskControlBlock* tcb);

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
void update(volatile T_TaskControlBlock** head, volatile T_TaskControlBlock* tcb, uint8_t priority);

/**
 * \brief Move a task control block from one linked list to another.
 *
 * This function moves a task control block from one linked list to another. It first removes the task
 * control block from the current linked list, and then inserts it into the new linked list.
 *
 * \param currentHead Pointer to the pointer to the head of the current linked list.
 * \param newHead Pointer to the pointer to the head of the new linked list.
 * \param tcb Pointer to the task control block to be moved.
 */
void move(volatile T_TaskControlBlock** currentHead, volatile T_TaskControlBlock** newHead, volatile T_TaskControlBlock* tcb);
__attribute__((naked)) void SysInit      (void);




#endif /* _OS_H_ */