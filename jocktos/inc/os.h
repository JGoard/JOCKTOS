/**
* \brief This module is to...
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
 * @brief Structure of a generic task default
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

#define CRITICAL_SECTION(arg) \
    do {                      \
        __disable_irq();      \
        arg;                  \
        __enable_irq();       \
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
/** 
 * @brief Cortex-M4 Context Control Block
 */

typedef void (*T_FunctionHandle)(void*);
typedef struct T_TaskControlBlock T_TaskControlBlock;
struct T_TaskControlBlock {
    /* ---- Configured ---*/
    uint8_t             u8Priority;          ///<    The priority of the task
    char*               u8Name;              ///<    Name of the tast
    uint32_t            u32StackSize_By;     ///<    Configured task stack size
    uint32_t            u32Delay;            ///<    Delay in ms on 
    /* ---- Input Data ---*/
    T_FunctionHandle    taskFunct;           ///<    Main function handle for task
    void*               taskArg;             ///<    Argument to be passed into the task function
    /* ---- Output Data---*/
    E_TaskState         eState;              ///<    Defines current task state
    /* ---s- Working Data--*/
    /* ---- Internal Data-*/
    uint32_t*           u32TaskStackOverflow; ///<    lowest accessible address for this tasks stack pointer
    uint32_t*           u32TaskStackPointer;  ///<    Hold's the current task stack pointer
    T_TaskControlBlock* TCBNext;              ///<    Next item for singly linked list
};


typedef struct {
    _Bool               active;    ///<    Initialization flag
    T_TaskControlBlock* running;   ///<    Currently running task
    T_TaskControlBlock* ready;     ///<    Singly linked list of tasks ready to run, in decending order of priority
    T_TaskControlBlock* blocked;   ///<    Singly linked list of blocked tasks, in decending order of priority
    T_TaskControlBlock* suspended; ///<    Singly linked list of suspended tasks, in decending order of priority
} T_Scheduler;


/* -- Externs (avoid these for library functions) ------------------------- */

/* -- Function Declarations ----------------------------------------------- */
_Bool createTask(T_TaskControlBlock* tcb);

void insert(T_TaskControlBlock** head, T_TaskControlBlock* tcb);
void remove(T_TaskControlBlock** head, T_TaskControlBlock* tcb);
void update(T_TaskControlBlock** head, T_TaskControlBlock* tcb, uint8_t priority);
__attribute__((naked)) void SysInit      (void);




#endif /* _OS_H_ */