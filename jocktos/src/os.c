/**
* \brief This module is to...
*/

/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "os.h"
#include "tcb.h"
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
/**    
 * Exception frame without floating-point storage 
 * 
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
/**
* \brief Place Holder OS tick handler
*
* Copies the Main Stack Pointer (MSP) and Process Stack Pointer (PSP) into globals
*
* \return
*/
void SysTick_Handler(void) {

    __asm volatile ("MRS %0, MSP" : "=r" (msp_test) :: "memory");
    __asm volatile ("MRS %0, PSP" : "=r" (psp_test) :: "memory");

}

void createTask(T_TaskControlBlock* tcb) {
    if (!tcb->taskFunct) {
        JOCKTOSError.invalidTaskHandle++;
        return;
    }
    uint32_t* taskStack =(uint32_t*) malloc(tcb->u32StackSize_By * sizeof(uint32_t));
    if (!taskStack) {
        JOCKTOSError.failedToAllocate++;
        return;
    }
    tcb->u32TaskStackOverflow = taskStack;
    insertTCB(&JOCKTOSScheduler.ready, tcb);
}

void switchTask(void) {
    // move the current running task into READY if it exists (not NULL)
    if (JOCKTOSScheduler.running) {
                JOCKTOSScheduler.running->eState = eREADY;
        insert(&JOCKTOSScheduler.ready, JOCKTOSScheduler.running);
    }
    // Pop task of the top of READY linked-list and move into running
    JOCKTOSScheduler.ready->eState = eRUNNING;
    JOCKTOSScheduler.running = JOCKTOSScheduler.ready;
    remove(&JOCKTOSScheduler.ready, JOCKTOSScheduler.running);
}


/* -- Public Functions----------------------------------------------------- */
