/**
* \brief This module is the core JOCKTOS kernel functionality
*/
/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "os.h"
#include "tcb.h"
#include "timers.h"
#include "allocator.h"
// Middleware
#include "stm32f303xe.h"
#include "core_cm4.h"
// Bios
// Standard C
#include <stdbool.h>
#include <stdlib.h>

/* -- Defines ------------------------------------------------------------- */

#define TRIGGER_PendSV *(uintptr_t volatile *)0xE000ED04 = (1U << 28)

/* -- Types --------------------------------------------------------------- */

/* -- Private Function Declarations --------------------------------------- */

/**
 * \brief Updates the task control blocks stack_usage
 * 
 * \param tcb Pointer to task control block to be monitored.
 */
static inline void monitorStackUsage(volatile TaskControlBlock** tcb) {
    (*tcb)->stack_usage = 100.0 * (1.0 - (sizeof(uintptr_t) * (double)((*tcb)->stack_pointer \
    - (*tcb)->stack_overflow)) / (double)((*tcb)->stack_size_bytes));
}

/**
 * @brief Initializes the stack for a task in a real-time operating system (RTOS).
 *
 * This function sets up the initial stack frame for the task by populating the stack with default values for various registers.
 * It also sets the program counter (PC) to the task function handle, the link register (LR) to a specific value, and the stack pointer (SP) to the bottom of the stack range.
 * After setting up the initial stack frame, the function fills the remaining unused stack space with a known value and sets the top 8 bytes to another known value.
 *
 * @param tcb Pointer to a TaskControlBlock structure representing the task.
 */
void initializeStack(TaskControlBlock* tcb);

/**
* \brief pre defined OS task for idle.
*
* Infinite while loop.
* TODO: Figure out how to low power sleep without disabling ISR's
*/
void idleTask(void* arg);

/**
 * \brief pre defined OS task to monitor stack usage
 */
void monitorTask(void* arg);

/* -- Local Globals (not for libraries with application instantiation) ---- */

static Allocator allocator;
Scheduler JOCKTOSScheduler = {false, 0, NULL, NULL, NULL};
extern TCBError JOCKTOS_TCBError;

TaskControlBlock usr_main_tcb = TASKCONTROLBLOCK_DEF(
        .task_handle=NULL,
        .name="user space `main`");

TaskControlBlock stack_monitor_tcb = TASKCONTROLBLOCK_DEF(
        .stack_size_bytes=1024, 
        .task_handle=monitorTask,
        .name="stack usage monitor");

TaskControlBlock idle_tcb = TASKCONTROLBLOCK_DEF(
        .stack_size_bytes=512,
        .task_handle=idleTask,
        .name="default OS idle task");

/* -- Public Functions----------------------------------------------------- */

void jock_createTask(TaskControlBlock* tcb) {
    // check if task function handle is valid
    if (!tcb->task_handle) {
        // TODO: better error handling
        JOCKTOS_TCBError.invalid_task_handle++;
        return;
    }
    tcb->stack_overflow = (uintptr_t*)allocate(&allocator, tcb->stack_size_bytes);
    if (!tcb->stack_overflow) {
        // TODO: better error handling
        JOCKTOS_TCBError.failed_to_allocate++;
        return;
    }
    initializeStack(tcb);
    insertTCB(&JOCKTOSScheduler.ready, tcb);
}

void switchRunningTask(volatile TaskControlBlock** head) {
    volatile TaskControlBlock* suspended = NULL;
    if (JOCKTOSScheduler.pending) return;
    JOCKTOSScheduler.pending = true;
    if (JOCKTOSScheduler.running) {
        insertTCB(head, JOCKTOSScheduler.running);  
    }
    suspended = JOCKTOSScheduler.suspended;
    while (suspended != NULL) {
        if (jock_currentTime() >= suspended->delay_ms) {
            moveTCB(&JOCKTOSScheduler.suspended, suspended, &JOCKTOSScheduler.ready);
            suspended->state = READY;
        }
        suspended = suspended->next;
    }
    TRIGGER_PendSV;
}

void jock_configure(JocktosConfig* config) {
    void* memory = calloc(ALLOCATOR_SIZE, sizeof(uint8_t));
    initAllocator(&allocator, config->allocator_block_size, memory, ALLOCATOR_SIZE);
    if (config->enable_monitor) jock_createTask(&stack_monitor_tcb);
    if (config->enable_main) insertTCB(&JOCKTOSScheduler.running, &usr_main_tcb);
    if (config->enable_idle || (!config->enable_monitor && !config->enable_main)) jock_createTask(&idle_tcb);
}

void jock_run(void) {
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /** failed attempt to utilize PSP Thread Mode
    uint32_t initPSP;
    uint32_t ctrl = 0x2; // set only the SPSEL bit
    __asm volatile ("mrs %0, msp" : "=r" (initPSP) );
    __asm volatile ("msr psp, %0" : : "r" (initPSP) : "memory");
    __asm volatile ("msr control, %0" : : "r" (ctrl) : "memory");
    __asm volatile ("isb"); // Required after modifications to special register MSP or PSP
    */
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    NVIC_SetPriority(PendSV_IRQn, 0xFFU);
    // Configure SysTick to generate an interrupt every 1 ms 
    sysTickConfiguration(127); // TODO: where tf does 127 come from...
    NVIC_SetPriority(SysTick_IRQn, 0U);
}

/* -- Private Functions --------------------------------------------------- */

void initializeStack(TaskControlBlock* tcb) {
    uintptr_t* stack_ptr = tcb->stack_overflow;
    // set intermediate stack pointer to bottom of range
    stack_ptr = (uintptr_t*)(stack_ptr + tcb->stack_size_bytes / sizeof(uintptr_t));
    // define tasks initial exception return stack
    uintptr_t* init_stack_ptr;
    //  - non-critical registers are initialized to their index
    *(--stack_ptr) = (1U << 24);                ///<   Set thumb state bit in EPSR
    *(--stack_ptr) = (uintptr_t)tcb->task_handle; ///<   Set PC to task function handle
    *(--stack_ptr) = 0xFFFFFFF9U;               ///<   Set LR  register for MSP thread mode
    // *(--stack_ptr) = 0xFFFFFFFDU;               ///<   Set LR  register for PSP thread mode
    *(--stack_ptr) = 0x0000000CU;               ///<   Set R12 register deafult to its index
    *(--stack_ptr) = 0x00000003U;               ///<   Set R3  register deafult to its index
    *(--stack_ptr) = 0x00000002U;               ///<   Set R2  register deafult to its index
    *(--stack_ptr) = 0x00000001U;               ///<   Set R1  register deafult to its index
    *(--stack_ptr) = (uintptr_t)tcb->task_arg;   ///<   Set R0  register to the argument for the tasks function
    init_stack_ptr = stack_ptr - 1;               ///<   Catch top of initial post-exception stack
    *(--stack_ptr) = (uintptr_t)init_stack_ptr;   ///<   ISR push / pop "working stack pointer" as R7
    *(--stack_ptr) = 0x0000000BU;               ///<   Set R11 register deafult to its index
    *(--stack_ptr) = 0x0000000AU;               ///<   Set R10 register deafult to its index
    *(--stack_ptr) = 0x00000009U;               ///<   Set R9  register deafult to its index
    *(--stack_ptr) = 0x00000008U;               ///<   Set R8  register deafult to its index
    *(--stack_ptr) = (uintptr_t)init_stack_ptr;   ///<   Set R7 to "working stack pointer"
    *(--stack_ptr) = 0x00000006U;               ///<   Set R6  register deafult to its index
    *(--stack_ptr) = 0x00000005U;               ///<   Set R5  register deafult to its index
    *(--stack_ptr) = 0x00000004U;               ///<   Set R4  register deafult to its index
    tcb->stack_pointer = stack_ptr;
    // Fill unused process stack with known value
    while (stack_ptr > tcb->stack_overflow + 8) {
        *(--stack_ptr) = 0xBABEFACEU;
    }
    while (stack_ptr > tcb->stack_overflow) {
        *(--stack_ptr) = 0xDEADBEEFU; ///< set top 8 bytes to something else
    }
}

void SysTick_Handler(void) {
    CRITICAL_SECTION(
        JOCKTOSScheduler.tick_count++;
        switchRunningTask(&JOCKTOSScheduler.ready);
   );
}

void PendSV_Handler(void) {
    CRITICAL_SECTION(
        if (JOCKTOSScheduler.running) {
            // --------------------------------------------------------------------------------------
            // push additional registers onto current process stack and store process stack pointer
            __asm volatile ("mrs r0, msp"); // TODO: figure out how to use PSP instead
            // __asm volatile ("mrs r0, psp");
            __asm volatile ("stmdb r0!, {r4-r11}");
            __asm volatile ("mov %0, r0" : "=r" (JOCKTOSScheduler.running->stack_pointer) :: );
            // --------------------------------------------------------------------------------------
        }
        // pop off of ready task list into running
        JOCKTOSScheduler.running = JOCKTOSScheduler.ready;
        JOCKTOSScheduler.ready = JOCKTOSScheduler.ready->next;
        JOCKTOSScheduler.running->next = NULL;
        JOCKTOSScheduler.running->state = RUNNING;
        JOCKTOSScheduler.pending = false;
        // ------------------------------------------------------------------------------------------
        // pop additional registers from the new process stack and load new process stack pointer
        __asm volatile ("mov r0, %0" : : "r" (JOCKTOSScheduler.running->stack_pointer) : "r0");
        __asm volatile ("ldmia r0!, {r4-r11}");
        __asm volatile ("msr msp, r0"); // TODO: figure out how to use PSP instead
        // __asm volatile ("msr psp, r0"); // TODO: figure out how to use PSP instead
        __asm volatile ("isb");         // Required after modifications to special register MSP (or PSP)
        // ------------------------------------------------------------------------------------------
    );
}

void monitorTask(void* arg) {
    volatile TaskControlBlock* head = NULL;
    TaskState monitor_scope = RUNNING;
    while(true) {
        switch (monitor_scope) {
            case READY: {
                head = JOCKTOSScheduler.ready;
                monitor_scope = RUNNING;
                break;
            }
            case RUNNING: {
                head = JOCKTOSScheduler.running;
                monitor_scope = SUSPENDED;
                break;
            }
            default: {
                head = JOCKTOSScheduler.suspended;
                monitor_scope = READY;
                break;
            }
        }
        while(head != NULL) {
            monitorStackUsage(&head);
            head = head->next;
        }
    }
}

void idleTask(void* arg) {
    while(true) {}
    // TODO: Figure out how to low power
}