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
 * \brief Updates the task control blocks stackUsage
 * 
 * \param tcb Pointer to task control block to be monitored.
 */
static inline void monitorStackUsage(volatile TaskControlBlock** tcb) {
    (*tcb)->stackUsage = 100.0 * (1.0 - ((double)((*tcb)->taskStackPointer \
    - (*tcb)->taskStackOverflow)) / (double)((*tcb)->stackSize_By * sizeof(uintptr_t)));
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
void idleJOCKTOS(void* arg);

/**
 * \brief pre defined OS task to monitor stack usage
 */
void monitorJOCKTOS(void* arg);

/* -- Local Globals (not for libraries with application instantiation) ---- */

static Allocator allocator;
Scheduler JOCKTOSScheduler = {false, 0, NULL, NULL, NULL};
extern TCBError JOCKTOS_TCBError;

TaskControlBlock userMainControlBlock = TASKCONTROLBLOCK_DEF(
        .taskFunct=NULL,
        .name="user space `main`");

TaskControlBlock stackUsageMonitor = TASKCONTROLBLOCK_DEF(
        .stackSize_By=1024, 
        .taskFunct=monitorJOCKTOS,
        .name="stack usage monitor");

TaskControlBlock defaultOSIdle = TASKCONTROLBLOCK_DEF(
        .stackSize_By=256, 
        .taskFunct=idleJOCKTOS,
        .name="default OS idle task");

/* -- Public Functions----------------------------------------------------- */

void createTask(TaskControlBlock* tcb) {
    // check if task function handle is valid
    if (!tcb->taskFunct) {
        // TODO: better error handling
        JOCKTOS_TCBError.invalidTaskHandle++;
        return;
    }
    tcb->taskStackOverflow = (uintptr_t*)allocate(&allocator, tcb->stackSize_By * sizeof(uintptr_t));
    if (!tcb->taskStackOverflow) {
        // TODO: better error handling
        JOCKTOS_TCBError.failedToAllocate++;
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
        if (currentTime() >= suspended->delay) {
            moveTCB(&JOCKTOSScheduler.suspended, &JOCKTOSScheduler.ready, suspended);
            suspended->state = READY;
        }
        suspended = suspended->TCBNext;
    }
    TRIGGER_PendSV;
}

void configureJOCKTOS(JocktosConfig* config) {
    void* memory = calloc(ALLOCATOR_SIZE, 1);
    initAllocator(&allocator, memory, ALLOCATOR_SIZE, config->allocatorBlockSize);
    if (config->enableMonitor) createTask(&stackUsageMonitor);
    if (config->enableMain) insertTCB(&JOCKTOSScheduler.running, &userMainControlBlock);
    if (config->enableIdle || (!config->enableMonitor && !config->enableMain)) createTask(&defaultOSIdle);
}

void runJOCKTOS(void) {
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
    SysTick_Configuration(127); // TODO: where tf does 127 come from...
    NVIC_SetPriority(SysTick_IRQn, 0U);
}

/* -- Private Functions --------------------------------------------------- */

void initializeStack(TaskControlBlock* tcb) {
    uintptr_t* taskStack = tcb->taskStackOverflow;
    // set intermediate stack pointer to bottom of range
    taskStack = (uintptr_t*)(taskStack + ((tcb->stackSize_By) / 8) * 8);
    // define tasks initial exception return stack
    uintptr_t* initStackPtr;
    //  - non-critical registers are initialized to their index
    *(--taskStack) = (1U << 24);                ///<   Set thumb state bit in EPSR
    *(--taskStack) = (uintptr_t)tcb->taskFunct; ///<   Set PC to task function handle
    *(--taskStack) = 0xFFFFFFF9U;               ///<   Set LR  register for MSP thread mode
    // *(--taskStack) = 0xFFFFFFFDU;               ///<   Set LR  register for PSP thread mode
    *(--taskStack) = 0x0000000CU;               ///<   Set R12 register deafult to its index
    *(--taskStack) = 0x00000003U;               ///<   Set R3  register deafult to its index
    *(--taskStack) = 0x00000002U;               ///<   Set R2  register deafult to its index
    *(--taskStack) = 0x00000001U;               ///<   Set R1  register deafult to its index
    *(--taskStack) = (uintptr_t)tcb->taskArg;   ///<   Set R0  register to the argument for the tasks function
    initStackPtr = taskStack - 1;               ///<   Catch top of initial post-exception stack
    *(--taskStack) = (uintptr_t)initStackPtr;   ///<   ISR push / pop "working stack pointer" as R7
    *(--taskStack) = 0x0000000BU;               ///<   Set R11 register deafult to its index
    *(--taskStack) = 0x0000000AU;               ///<   Set R10 register deafult to its index
    *(--taskStack) = 0x00000009U;               ///<   Set R9  register deafult to its index
    *(--taskStack) = 0x00000008U;               ///<   Set R8  register deafult to its index
    *(--taskStack) = (uintptr_t)initStackPtr;   ///<   Set R7 to "working stack pointer"
    *(--taskStack) = 0x00000006U;               ///<   Set R6  register deafult to its index
    *(--taskStack) = 0x00000005U;               ///<   Set R5  register deafult to its index
    *(--taskStack) = 0x00000004U;               ///<   Set R4  register deafult to its index
    tcb->taskStackPointer = taskStack;
    // Fill unused process stack with known value
    while (taskStack > tcb->taskStackOverflow + 8) {
        *(--taskStack) = 0xBABEFACEU;
    }
    while (taskStack > tcb->taskStackOverflow) {
        *(--taskStack) = 0xDEADBEEFU; ///< set top 8 bytes to something else
    }
}

void SysTick_Handler(void) {
    __asm volatile ("cpsid i" : : : "memory");
    JOCKTOSScheduler.tickCount++;
    switchRunningTask(&JOCKTOSScheduler.ready);
   __asm volatile ("cpsie i" : : : "memory");
}

void PendSV_Handler(void) {
    __asm volatile ("cpsid i" : : : "memory");
    if (JOCKTOSScheduler.running) {
        // --------------------------------------------------------------------------------------
        // push additional registers onto current process stack and store process stack pointer
        __asm volatile ("mrs r0, msp"); // TODO: figure out how to use PSP instead
        // __asm volatile ("mrs r0, psp");
        __asm volatile ("stmdb r0!, {r4-r11}");
        __asm volatile ("mov %0, r0" : "=r" (JOCKTOSScheduler.running->taskStackPointer) :: );
        // --------------------------------------------------------------------------------------
    }
    // pop off of ready task list into running
    JOCKTOSScheduler.running = JOCKTOSScheduler.ready;
    JOCKTOSScheduler.ready = JOCKTOSScheduler.ready->TCBNext;
    JOCKTOSScheduler.running->TCBNext = NULL;
    JOCKTOSScheduler.running->state = RUNNING;
    JOCKTOSScheduler.pending = false;
    // ------------------------------------------------------------------------------------------
    // pop additional registers from the new process stack and load new process stack pointer
    __asm volatile ("mov r0, %0" : : "r" (JOCKTOSScheduler.running->taskStackPointer) : "r0");
    __asm volatile ("ldmia r0!, {r4-r11}");
    __asm volatile ("msr msp, r0"); // TODO: figure out how to use PSP instead
    // __asm volatile ("msr psp, r0"); // TODO: figure out how to use PSP instead
    __asm volatile ("isb");         // Required after modifications to special register MSP (or PSP)
    // ------------------------------------------------------------------------------------------
    __asm volatile ("cpsie i" : : : "memory");
}

void monitorJOCKTOS(void* arg) {
    volatile TaskControlBlock* head = NULL;
    TaskState monitorScope = RUNNING;
    while(true) {
        switch (monitorScope) {
            case READY: {
                head = JOCKTOSScheduler.ready;
                monitorScope = RUNNING;
                break;
            }
            case RUNNING: {
                head = JOCKTOSScheduler.running;
                monitorScope = SUSPENDED;
                break;
            }
            default: {
                head = JOCKTOSScheduler.suspended;
                monitorScope = READY;
                break;
            }
        }
        while(head != NULL) {
            monitorStackUsage(&head);
            head = head->TCBNext;
        }
    }
}

void idleJOCKTOS(void* arg) {
    while(true) {}
    // TODO: Figure out how to low power
}