/**
* \brief This header is to act as companion header for os.c
*/
#pragma once
/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "tcb.h"
#include "errors.h"
// Middleware
#include "cmsis_gcc.h"
#include "buffer.h"
// Bios
// Standard C
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* -- Defines ------------------------------------------------------------- */

/** 
 * @brief enable / disablt ISR wrapper
 */
#define CRITICAL_SECTION(primask, ...)          \
    __disable_irq();                            \
        __VA_ARGS__                             \
    __enable_irq();                             \


///<TODO:Need to figure out why __get__PRIMASK crashes PENDSV Handler
// /** 
//  * @brief enable / disable ISR wrapper   
//  */
// Below is commented out, even though it seems like its not. I swear
/*
 #define CRITICAL_SECTION(primask, ...)          \
     primask = __get_PRIMASK();                  \
     __disable_irq();                            \
     do{                                         \   
         __VA_ARGS__                             \
     }                                           \
     while(0);                                   \
     if (primask == 0) {                         \
     __enable_irq();                             \
     }                                           \
*/
/** 
 * @brief Default JOCKTOS configuration
 */
#define JOCKTOSCONFIG_DEF(...)      \
{                                   \
    .enable_monitor      = false,   \
    .enable_main         = false,   \
    .enable_idle         = false,   \
    .allocator_block_size = 256,    \
    .logger_size         = 256,     \
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
    volatile TaskControlBlock* terminated;  ///<    Currently running task
    volatile TaskControlBlock* running;     ///<    Currently running task
    volatile TaskControlBlock* ready;       ///<    Singly linked list of tasks ready to run, in decending order of priority
    volatile TaskControlBlock* suspended;   ///<    Singly linked list of suspended tasks, in decending order of priority
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
    bool enable_monitor;           ///< Enable or disable monitoring
    bool enable_idle;              ///< Enable or disable idle task
    bool enable_main;              ///< return execution after enabling, with `main` considered a new task
    uint16_t allocator_block_size; ///< Size of the allocator block
    uint16_t logger_size;          ///< Number of log messages to buffer
} JocktosConfig;

/* -- Externs (avoid these for library functions) ------------------------- */

extern Scheduler JOCKTOSScheduler;
extern Buffer* JOCKTOS_log_buffer;

/* -- Function Declarations ----------------------------------------------- */

/**
 * \brief Create a new task and add it to the scheduler.
 *
 * This function creates a new task by allocating memory for its stack, initializes the task stack pointer,
 * and inserts the task control block into the scheduler's ready queue.
 *
 * \param tcb Pointer to the task control block representing the new task.
 */
void jock_os_install_task(TaskControlBlock* tcb);

/**
 * \brief Switch the currently running task
 *
 * Updates the Schedulers linked lists and task states upon context switch 
 *
 * \param head Pointer to destination for current running task.
 */
void jock_os_switch_running_task(volatile TaskControlBlock** head);

/**
 * \brief configure / enable built in OS tasks
 */
void jock_os_configure(JocktosConfig* config);

/**
 * \brief Enable scheduler and context switching ISR's
 *
 * Sets the priorities and enables systick and pendSV handlers
 *
 */
void jock_os_run(void);

/**
 * \brief returns the current OS tick count
 * 
 * unsigned 32 bit millisecond counter
 * 
 */
static inline uint32_t jock_os_get_time() { return JOCKTOSScheduler.tick_count; }

/**
 * \brief Lock interrupts to start a critical section.
 *
 * When used with jock_os_leave_critical_section() this function starts a critical section that
 * works properly even if nested in another critical section because it reads
 * the PRIMASK value so it can be restored.
 *
 * @return the priority mask (PRIMASK) register value upon entry
 */
uint32_t jock_os_enter_critical_section(void); ///<TODO: Maybe we can expose this

/**
 *  \brief Unlock interrupts to end a critical section.
 *
 * When used with jock_os_enter_critical_section() this function ends a critical section that works
 * properly even if nested in another critical section because it uses the
 * previous interrupt locking state (defined by PRIMASK) to selectively unlock
 * interrupts.
 *
 * @param primask   The previous priority mask (PRIMASK) register value as
 *                  returned by jock_os_enter_critical_section().
 * @return none
 */
void jock_os_leave_critical_section(uint32_t primask);

/**
 * \brief Log a message to the OS buffer
 * 
 * \param message Pointer to the message to be logged
 */
static inline void jock_os_log(JOCKTOSMessage* message) {
    bufferWrite(JOCKTOS_log_buffer, message);
}

/**
 * \details Safely catches an unexpected return from a task.
 * Any task that returns is marked as 'terminated' and moved accordingly.
 * 
 */
void jock_os_kill_task(void);

/**
 * \brief Updates the task control blocks stack_usage
 * 
 * \param tcb Pointer to task control block to be monitored.
 */
static inline void monitorStackUsage(volatile TaskControlBlock** tcb) {
    double stack_bytes_free = (double)((uint8_t*)(*tcb)->stack_pointer - (*tcb)->stack_overflow);
    double free_ratio = stack_bytes_free / (double)(*tcb)->stack_size_bytes;
    (*tcb)->stack_usage = 100.0 * (1.0 - free_ratio);
}
