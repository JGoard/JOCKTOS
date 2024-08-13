/**
* \brief This header is to act as companion header for os.c
*/
#ifndef _OS_H_
#define _OS_H_
/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "tcb.h"
// Middleware
// Bios
// Standard C
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* -- Defines ------------------------------------------------------------- */

/** 
 * @brief enable / disablt ISR wrapper
 */
#define CRITICAL_SECTION(...)                      \
    do {                                           \
        __asm volatile ("cpsid i" : : : "memory"); \
        __VA_ARGS__;                               \
        __asm volatile ("cpsie i" : : : "memory"); \
    } while (0)


/** 
 * @brief Default JOCKTOS configuration
 */
#define JOCKTOSCONFIG_DEF(...)      \
{                                   \
    .enable_monitor      = false,   \
    .enable_main         = false,   \
    .enable_idle         = false,   \
    .allocator_block_size = 256,    \
     __VA_ARGS__                    \
}
/* -- Types --------------------------------------------------------------- */

/**
 * @brief Cortex-M4 Context Control Block
 *
 * The T_Scheduler struct is used to maintain the state of the scheduler.
 * It contains pointers to the currently running task, a singly linked list
 * of tasks ready to run, and a singly linked list of suspended tasks. The
 * tickCount member is used to keep track of the number of ticks since the
 * scheduler was last run.
 *
 * The pending member is used to signal that a context switch is pending.
 * This allows the scheduler to be called from interrupt handlers.
 *
 * The running member is a pointer to the T_TaskControlBlock that is
 * currently running. It is used to modify the state of the current task,
 * such as changing its priority or suspending it.
 *
 * The ready and suspended members are pointers to the head of the singly
 * linked lists of tasks ready to run and suspended, respectively. These
 * are used to modify the state of these tasks.
 *
 * The T_Scheduler struct is defined as a volatile, as it is used in ISRs
 * to signal that a context switch is pending. This ensures that the
 * compiler does not optimize away reads and writes to this struct.
 */

typedef struct {
    volatile bool pending;
    volatile uint32_t tick_count;
    volatile TaskControlBlock* running;   ///<    Currently running task
    volatile TaskControlBlock* ready;     ///<    Singly linked list of tasks ready to run, in decending order of priority
    volatile TaskControlBlock* suspended; ///<    Singly linked list of suspended tasks, in decending order of priority
} Scheduler;

/**
 * @brief This structure is used to configure the JOCKTOS library.  It is passed to the
 *        `Jocktos_Init` function and is used to configure how the library operates.
 * 
 * @details This structure is used to configure the JOCKTOS library.  It is passed to
 *          the `Jocktos_Init` function and is used to configure how the library operates.
 *          The configuration options include enabling or disabling the JOCKTOS
 *          monitoring task, enabling or disabling the idle task, and enabling or
 *          disabling the main task.  The `allocatorBlockSize` option is used to
 *          configure the size of the memory blocks that are allocated for the
 *          tasks' stacks.  If this size is too small then the tasks will not be
 *          able to run and the library will not work properly.  This value should
 *          be set to a reasonable value based on the size of the tasks and the
 *          amount of memory available on the system.
 */
typedef struct {
    bool enable_monitor;         ///< Enable or disable monitoring
    bool enable_idle;            ///< Enable or disable idle task
    bool enable_main;            ///< return execution after enabling, with `main` considered a new task
    size_t allocator_block_size; ///< Size of the allocator block
} JocktosConfig;

/* -- Externs (avoid these for library functions) ------------------------- */

extern Scheduler JOCKTOSScheduler;

/* -- Function Declarations ----------------------------------------------- */

/**
 * \brief Create a new task and add it to the scheduler.
 *
 * This function creates a new task by allocating memory for its stack, initializes the task stack pointer,
 * and inserts the task control block into the scheduler's ready queue.
 *
 * \param tcb Pointer to the task control block representing the new task.
 */
void jock_createTask(TaskControlBlock* tcb);

/**
 * \brief Switch the currently running task
 *
 * Updates the Schedulers linked lists and task states upon context switch 
 *
 * \param head Pointer to destination for current running task.
 */
void switchRunningTask(volatile TaskControlBlock** head);

/**
 * \brief configure / enable built in OS tasks
 */
void jock_configure(JocktosConfig* config);

/**
 * \brief Enable scheduler and context switching ISR's
 *
 * Sets the priorities and enables systick and pendSV handlers
 *
 */
void jock_run(void);

/**
 * \brief returns the current OS tick count
 * 
 * unsigned 32 bit millisecond counter
 * 
 */
static inline uint32_t jock_currentTime() { return JOCKTOSScheduler.tick_count; }

#endif /* _OS_H_ */
