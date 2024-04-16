/**
* \brief This module is to...
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
#include <stdlib.h>
/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Local Globals (not for libraries with application instantiation) ---- */
T_Scheduler JOCKTOSScheduler;
extern T_TCBError JOCKTOS_TCBError;

/* -- Private Function Declarations --------------------------------------- */
void SysTick_Handler(void) {
    CRITICAL_SECTION(
        switchRunningTask();
   );
}

void PendSV_Handler(void) {
    CRITICAL_SECTION(
        if (JOCKTOSScheduler.running) {
            // --------------------------------------------------------------------------------------
            // push additional registers onto current process stack and store process stack pointer
            __asm volatile ("mrs r0, msp"); // TODO: figure out how to use PSP instead
            __asm volatile ("stmdb r0!, {r4-r11}");
            __asm volatile ("mov %0, r0" : "=r" (JOCKTOSScheduler.running->u32TaskStackPointer) :: );
            // --------------------------------------------------------------------------------------
        }
        // pop off of ready task list into running
        JOCKTOSScheduler.running = JOCKTOSScheduler.ready;
        JOCKTOSScheduler.ready = JOCKTOSScheduler.ready->TCBNext;
        JOCKTOSScheduler.running->TCBNext = NULL;
        JOCKTOSScheduler.running->eState = eRUNNING;
        // ------------------------------------------------------------------------------------------
        // pop additional registers from the new process stack and load new process stack pointer
        __asm volatile ("mov r0, %0" : : "r" (JOCKTOSScheduler.running->u32TaskStackPointer) : "r0");
        __asm volatile ("ldmia r0!, {r4-r11}");
        __asm volatile ("msr msp, r0"); // TODO: figure out how to use PSP instead
        __asm volatile ("isb");         // Required after modifications to special register MSP (or PSP)
        // ------------------------------------------------------------------------------------------
    );
}

 /** Exception frame without floating-point storage 
 *        ...      <--\
 *  ---------------    |--- Pre-IRQ top of stack
 *     {aligner}   <--/
 *  ---------------
 *       xPSR 
 *  ---------------
 *        PC
 *  ---------------
 *        LR
 *  ---------------
 *        R12
 *  ---------------
 *        R3
 *  ---------------
 *        R2
 *  ---------------
 *        R1
 *  ---------------
 *        R0
 *  ---------------  
 *       ...      <----- IRQ top of stack
*/
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
void switchRunningTask(void) {
    volatile T_TaskControlBlock** head = NULL;
    // move the current running task into READY if it exists (not NULL)
    if (JOCKTOSScheduler.running) {
        switch (JOCKTOSScheduler.running->eState) {
            case eBLOCKED: {
                head = &JOCKTOSScheduler.blocked;
                break;
            }
            case eSUSPENDED: {
                head = &JOCKTOSScheduler.suspended;
                break;
            }
            default: {
                head = &JOCKTOSScheduler.ready;
                JOCKTOSScheduler.running->eState = eREADY;
                break;
            }
        }
        insertTCB(head, JOCKTOSScheduler.running);
        monitorStackUsage(&JOCKTOSScheduler.running);
        
    }
    TRIGGER_PendSV;
}

/* -- Public Functions----------------------------------------------------- */
void runJOCKTOS(void) {
    NVIC_SetPriority(PendSV_IRQn, 0xFFU);
    SysTick_Configuration(1000);
    NVIC_SetPriority(SysTick_IRQn, 0U);
    for(;;) {} // TODO: implement OS idle task
}
