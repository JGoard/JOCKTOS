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
#define ALLOCATOR_SIZE 8192 // TODO: redefine this to use full heap as per the linker file
/* -- Types --------------------------------------------------------------- */

/* -- Local Globals (not for libraries with application instantiation) ---- */
static Allocator allocator;
T_Scheduler JOCKTOSScheduler = {false, 0, NULL, NULL, NULL};
extern T_TCBError JOCKTOS_TCBError;

T_TaskControlBlock userMainControlBlock = T_TASKCONTROLBLOCK_DEF(
        .taskFunct=NULL,
        .u8Name="user space `main`");

T_TaskControlBlock stackUsageMonitor = T_TASKCONTROLBLOCK_DEF(
        .u32StackSize_By=1024, 
        .taskFunct=monitorJOCKTOS,
        .u8Name="stack usage monitor");

T_TaskControlBlock defaultOSIdle = T_TASKCONTROLBLOCK_DEF(
        .u32StackSize_By=256, 
        .taskFunct=idleJOCKTOS,
        .u8Name="default OS idle task");

/* -- Private Function Declarations --------------------------------------- */

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
        __asm volatile ("mov %0, r0" : "=r" (JOCKTOSScheduler.running->u32TaskStackPointer) :: );
        // --------------------------------------------------------------------------------------
    }
    // pop off of ready task list into running
    JOCKTOSScheduler.running = JOCKTOSScheduler.ready;
    JOCKTOSScheduler.ready = JOCKTOSScheduler.ready->TCBNext;
    JOCKTOSScheduler.running->TCBNext = NULL;
    JOCKTOSScheduler.running->eState = eRUNNING;
    JOCKTOSScheduler.pending = false;
    // ------------------------------------------------------------------------------------------
    // pop additional registers from the new process stack and load new process stack pointer
    __asm volatile ("mov r0, %0" : : "r" (JOCKTOSScheduler.running->u32TaskStackPointer) : "r0");
    __asm volatile ("ldmia r0!, {r4-r11}");
    __asm volatile ("msr msp, r0"); // TODO: figure out how to use PSP instead
    // __asm volatile ("msr psp, r0"); // TODO: figure out how to use PSP instead
    __asm volatile ("isb");         // Required after modifications to special register MSP (or PSP)
    // ------------------------------------------------------------------------------------------
    __asm volatile ("cpsie i" : : : "memory");
}
void initializeStack(T_TaskControlBlock* tcb) {
    uintptr_t* taskStack = tcb->u32TaskStackOverflow;
    // set intermediate stack pointer to bottom of range
    taskStack = (uintptr_t*)(taskStack + ((tcb->u32StackSize_By) / 8) * 8);
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
    *(--taskStack) = 0x00000000U;               ///<   Set R0  register deafult to its index
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
    tcb->u32TaskStackPointer = taskStack;
    // Fill unused process stack with known value
    while (taskStack > tcb->u32TaskStackOverflow + 8) {
        *(--taskStack) = 0xBABEFACEU;
    }
    while (taskStack > tcb->u32TaskStackOverflow) {
        *(--taskStack) = 0xDEADBEEFU; ///< set top 8 bytes to something else
    }
}

void switchRunningTask(volatile T_TaskControlBlock** head) {
    volatile T_TaskControlBlock* suspended = NULL;
    if (JOCKTOSScheduler.pending) return;
    JOCKTOSScheduler.pending = true;
    if (JOCKTOSScheduler.running) {
        insertTCB(head, JOCKTOSScheduler.running);  
    }
    suspended = JOCKTOSScheduler.suspended;
    while (suspended != NULL) {
        if (currentTime() >= suspended->u32Delay) {
            moveTCB(&JOCKTOSScheduler.suspended, &JOCKTOSScheduler.ready, suspended);
            suspended->eState = eREADY;
        }
        suspended = suspended->TCBNext;
    }
    TRIGGER_PendSV;
}

void monitorJOCKTOS(uintptr_t* new_sp) {
    volatile T_TaskControlBlock* head = NULL;
    E_TaskState monitorScope = eRUNNING;
    while(true) {
        switch (monitorScope) {
            case eREADY: {
                head = JOCKTOSScheduler.ready;
                monitorScope = eRUNNING;
                break;
            }
            case eRUNNING: {
                head = JOCKTOSScheduler.running;
                monitorScope = eSUSPENDED;
                break;
            }
            default: {
                head = JOCKTOSScheduler.suspended;
                monitorScope = eREADY;
                break;
            }
        }
        while(head != NULL) {
            monitorStackUsage(&head);
            head = head->TCBNext;
        }
    }
}

void idleJOCKTOS(uintptr_t* new_sp) {
    while(true) {}
    // TODO: Figure out how to low power
}
/* -- Public Functions----------------------------------------------------- */

void createTask(T_TaskControlBlock* tcb) {
    // check if task function handle is valid
    if (!tcb->taskFunct) {
        // TODO: better error handling
        JOCKTOS_TCBError.invalidTaskHandle++;
        return;
    }
    tcb->u32TaskStackOverflow = (uintptr_t*)allocate(&allocator, tcb->u32StackSize_By * sizeof(uintptr_t));
    if (!tcb->u32TaskStackOverflow) {
        // TODO: better error handling
        JOCKTOS_TCBError.failedToAllocate++;
        return;
    }
    initializeStack(tcb);
    insertTCB(&JOCKTOSScheduler.ready, tcb);
}

void configureJOCKTOS(T_JocktosConfig* config) {
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
