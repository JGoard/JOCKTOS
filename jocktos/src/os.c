/**
* \brief This module is the core JOCKTOS kernel functionality
*/
/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "os.h"
#include "tcb.h"
#include "timers.h"
// Middleware
#include "stm32f303xe.h"
#include "core_cm4.h"
// Bios
// Standard C
#include <stdbool.h>
#include <stdlib.h>

/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Local Globals (not for libraries with application instantiation) ---- */

T_Scheduler JOCKTOSScheduler;
extern T_TCBError JOCKTOS_TCBError;

/* -- Private Function Declarations --------------------------------------- */

void SysTick_Handler(void) {
    __asm volatile ("cpsid i" : : : "memory");
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

void createTask(T_TaskControlBlock* tcb) {
    // initial post-exception stack pointer
    uint32_t* initStackPtr = NULL;
    // check if task function handle is valid
    if (!tcb->taskFunct) {
        // TODO: better error handling
        JOCKTOS_TCBError.invalidTaskHandle++;
        return;
    }
    uint32_t* taskStack =(uint32_t*) malloc(tcb->u32StackSize_By * sizeof(uint32_t));
    if (!taskStack) {
        // TODO: better error handling
        JOCKTOS_TCBError.failedToAllocate++;
        return;
    }
    // set [currently unused] overflow pointer to the top of allocated range
    tcb->u32TaskStackOverflow = taskStack;
    // set intermediate stack pointer to bottom of range
    taskStack = (uint32_t*)(taskStack + ((tcb->u32StackSize_By) / 8) * 8);
    // define tasks initial exception return stack
    //  - non-critical registers are initialized to their index
    *(--taskStack) = (1U << 24);                ///<   Set thumb state bit in EPSR
    *(--taskStack) = (uint32_t)(tcb->taskFunct); ///<   Set PC to task function handle
    *(--taskStack) = 0xFFFFFFF9U;               ///<   Set LR  register for MSP thread mode
    // *(--taskStack) = 0xFFFFFFFDU;               ///<   Set LR  register for PSP thread mode
    *(--taskStack) = 0x0000000CU;               ///<   Set R12 register deafult to its index
    *(--taskStack) = 0x00000003U;               ///<   Set R3  register deafult to its index
    *(--taskStack) = 0x00000002U;               ///<   Set R2  register deafult to its index
    *(--taskStack) = 0x00000001U;               ///<   Set R1  register deafult to its index
    *(--taskStack) = 0x00000000U;               ///<   Set R0  register deafult to its index
    initStackPtr = taskStack - 1;               ///<   Catch top of initial post-exception stack
    *(--taskStack) = (uint32_t)initStackPtr;    ///<   ISR push / pop "working stack pointer" as R7
    *(--taskStack) = 0x0000000BU;               ///<   Set R11 register deafult to its index
    *(--taskStack) = 0x0000000AU;               ///<   Set R10 register deafult to its index
    *(--taskStack) = 0x00000009U;               ///<   Set R9  register deafult to its index
    *(--taskStack) = 0x00000008U;               ///<   Set R8  register deafult to its index
    *(--taskStack) = (uint32_t)initStackPtr;    ///<   Set R7 to "working stack pointer"
    *(--taskStack) = 0x00000006U;               ///<   Set R6  register deafult to its index
    *(--taskStack) = 0x00000005U;               ///<   Set R5  register deafult to its index
    *(--taskStack) = 0x00000004U;               ///<   Set R4  register deafult to its index
    tcb->u32TaskStackPointer = taskStack;
    // Fill unused process stack with known value
    while (taskStack > tcb->u32TaskStackOverflow + 8) {
        *(--taskStack) = 0xBABEFACEU;
    }
    while (taskStack >= tcb->u32TaskStackOverflow) {
        *(--taskStack) = 0xDEADBEEFU; ///< set top 8 bytes to something else
    }
    insertTCB(&JOCKTOSScheduler.ready, tcb);
}

// possibly premature abstraction
void switchRunningTask(volatile T_TaskControlBlock** head) {
    if (JOCKTOSScheduler.pending) return;
    JOCKTOSScheduler.pending = true;
    // move the current running task into READY if it exists (not NULL)
    if (JOCKTOSScheduler.running) {
        insertTCB(head, JOCKTOSScheduler.running);
        monitorStackUsage(&JOCKTOSScheduler.running);
        
    }
    TRIGGER_PendSV;
}

/* -- Public Functions----------------------------------------------------- */

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
    SysTick_Configuration(1000);
    NVIC_SetPriority(SysTick_IRQn, 0U);
    for(;;) {} // TODO: implement OS idle task
}
