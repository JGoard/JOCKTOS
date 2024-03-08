/**
* \brief This module contains the main function and etc
*/

/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "main.h"
// Middleware
// Bios
// Standard C

/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Local Globals (not for libraries with application instantiation) ---- */

/* -- Private Function Declarations --------------------------------------- */

/* -- Public Functions----------------------------------------------------- */

/**
* \brief Define function...
*
* Lorem Ipsum
*
* \return
*/
int main(void)
{
    int x = 0;
    int y = 100;
    
    while(1)
    {
        x++;
        if (x >= 100) x = 0;
         y--;
        if (y <= 0) y = 100;
    };
}

/**
 * @brief   It is intended to use Cortex M's systick interrupt as the periodic interrupt intended for swapping
 *          tasks. The processor will save and restor some CPU registers as part of the interrupt's entry and exit
 *          strategy.
 * 
 *          The CPU will push 8 registers ( PSR, PC, LR, R12, R3, R2 R1, and R0) onto the stack on an exception.
 *          The exception routine is executed. When the exception returns, the CPU will pop 8 words that are the 
 *          previously mentioned registers from the stack and loads them into the CPU registers in the same order
 *          unto which they were saved.
 * 
 *          This interrupt is intended to save and restore the other registers (R4, R5, R6, R7, R8, R9, R10 & R11) 
 *          within the interrupt. Since the SP will be stored in the task control block, we do not need to push them
 *          to the stack. Below is this implementatiom
 * 
 *          Adding __attribute__((naked)) suppresses the generation of assembly code that allocates/deallocates space 
 *          for auto variables in the program stack during the entry/exit of the function.  This attribute ensures that the
 *          stack is not altered between the interrupt and executino occurence of the ISR.
 * 
 *          This function assumes that the each TCB will include a default task context even during the first task context
 *          switch. The following requirements must be met in order to initialize the task stack...
 *          *   Set the tasks stack pointer(in tcbs) such that it points to the top of the task context
 *              (equivalent to have 16 words stored in the task stack).   
 *          *   Copy 0x01000000 to xPSR in the stacked context. This makes sure that the processor knows that it needs to run 
 *              on thumb mode when the value is copied from to xPSR register.
 *          *   Copy the address of the function that implements the task to the stacked PC.
 * \return: none
 */
__attribute__((naked)) void SysTick_Handler(void)
{
/**************************************************/
/**--- STEP 1 - SAVE THE CURRENT TASK CONTEXT ---**/
/**************************************************/

    /**
    At this point the processor has already pushed PSR, PC, LR, R12, R3, R2, R1 and R0
    onto the stack. We need to push the rest(i.e R4, R5, R6, R7, R8, R9, R10 & R11) to save the
    context of the current task.
    Disable interrupts
    */
	
    
    __asm("CPSID   I");
    // Push registers R4 to R7
    __asm("PUSH    {R4-R7}");
    // Push registers R8-R11
    __asm("MOV     R4, R8");
    __asm("MOV     R5, R9");
    __asm("MOV     R6, R10");
    __asm("MOV     R7, R11");
    __asm("PUSH    {R4-R7}");
    // Load R0 with the address of pCurntTcb
    // __asm("LDR     R0, =pCurntTcb");
    // Load R1 with the content of pCurntTcb(i.e post this, R1 will contain the address of current TCB).
    __asm("LDR     R1, [R0]");
    // Move the SP value to R4
    __asm("MOV     R4, SP");
    // Store the value of the stack pointer(copied in R4) to the current tasks "stackPt" element in its TCB.
    // This marks an end to saving the context of the current task.
    __asm("STR     R4, [R1]");

/***************************************************************************************************/
/**--- STEP 2: LOAD THE NEW TASK CONTEXT FROM ITS STACK TO THE CPU REGISTERS, UPDATE pCurntTcb ---**/
/***************************************************************************************************/
    // Load the address of the next task TCB onto the R1.
    __asm("LDR     R1, [R1,#4]");
    // Load the contents of the next tasks stack pointer to pCurntTcb, equivalent to pointing pCurntTcb to
    // the newer tasks TCB. Remember R1 contains the address of pCurntTcb.
    __asm("STR     R1, [R0]");
    // Load the newer tasks TCB to the SP using R4.
    __asm("LDR     R4, [R1]");
    __asm("MOV     SP, R4");
    // Pop registers R8-R11
    __asm("POP     {R4-R7}");
    __asm("MOV     R8, R4");
    __asm("MOV     R9, R5");
    __asm("MOV     R10, R6");
    __asm("MOV     R11, R7");
    // Pop registers R4-R7
    __asm("POP     {R4-R7}");
    __asm("CPSIE   I ");
    __asm("BX      LR");
}