/**
* \brief This module is to...
*/

/* -- Includes ------------------------------------------------------------ */
// Jocktos
// Middleware
// Bios
// Standard C
#include "timers.h"
#include "system_stm32f3xx.h"
/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Local Globals (not for libraries with application instantiation) ---- */

/* -- Private Function Declarations --------------------------------------- */

/* -- Public Functions----------------------------------------------------- */
void SysTick_Configuration(void) {
    // Configure SysTick to generate an interrupt every 1 ms
    SysTick->LOAD = (SystemCoreClock / 1000) - 1; // Set the reload value for a 1ms interrupt
    SysTick->VAL = 0;                             // Clear the current value
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
}
/**
* \brief Define function...
*
* Lorem Ipsum
*
* \return
*/

void SysTick_Handler(void) {
    __asm volatile ("MRS %0, PSP" : "=r" (psp_test) :: "memory");
    __asm volatile ("MRS %0, MSP" : "=r" (msp_test) :: "memory");
    UsrSysTickHander();
}

void initTaskAndEnablePSP(uint32_t* taskStack, uint32_t stackSize) {
    // Enable the PSP by setting SPSEL bit (bit 1) in the CONTROL register
    __asm volatile("MRS R0, CONTROL");
    __asm volatile("ORR R0, R0,  #2"); 
    __asm volatile("MSR CONTROL, R0");

    // Set the PSP register to the initial stack pointer value
    __asm volatile ("MSR PSP, %0": : "r" (taskStack + stackSize - 4));
}
