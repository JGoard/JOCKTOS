#include "exception_handlers.h"
#include "os.h"
#include "tcb.h"

#include "bsp.h"
#include "cmsis_gcc.h"

extern Scheduler JOCKTOSScheduler;
/**
 * @brief This is the SysTick Interrupt Service Routine (ISR).
 *
 * This function is called by the SysTick timer every millisecond. It increments
 * the tickCount variable in the JOCKTOS scheduler, and then calls the
 * switchRunningTask function to check if any tasks are ready to run. If there
 * are tasks ready to run, this function will switch to the highest priority task
 * and begin executing it.
 *
 * @return None
 */
void SysTick_Handler(void) {
    uint32_t primask;
    primask = jock_os_enter_critical_section();
    JOCKTOSScheduler.tick_count++;
    jock_os_switch_running_task(&JOCKTOSScheduler.ready);
    jock_os_leave_critical_section(primask);
}

__attribute__((optimize("O0")))
void PendSV_Handler(void) {
    uint32_t primask;
    // primask = __get_PRIMASK();
    __disable_irq();
    if (JOCKTOSScheduler.running) {
        // --------------------------------------------------------------------------------------
        // push additional registers onto current process stack and store process stack pointer
        __asm volatile ("mrs r0, psp" ::: "memory");
        __asm volatile ("stmdb r0!, {r4-r11}" ::: "memory");
        __asm volatile ("mov %0, r0" : "=r" (JOCKTOSScheduler.running->stack_pointer) :: "memory");
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
    __asm volatile ("mov r0, %0" : : "r" (JOCKTOSScheduler.running->stack_pointer) : "r0", "memory");
    __asm volatile ("ldmia r0!, {r4-r11}" ::: "memory");
    __asm volatile ("msr psp, r0" ::: "memory");
    __asm volatile ("mrs r7, msp" ::: "memory"); // because SP gets set to R7(!?!?) in the irq return
    __asm volatile ("isb" ::: "memory");           // Required after modifications to special register MSP (or PSP)
    // ------------------------------------------------------------------------------------------

    // if(primask == 0) {
    __enable_irq();
    
// }
}

void MemManage_Handler(void) { 
    SCB->CFSR &= 0xFFu;
    TaskControlBlock* tcb = JOCKTOSScheduler.running;
    tcb->stack_pointer = __get_PSP();
    monitorStackUsage(&tcb);
    uintptr_t* stack_ptr = tcb->stack_overflow;
    // set intermediate stack pointer to bottom of range
    stack_ptr = (uintptr_t*)(stack_ptr + tcb->stack_size_bytes / sizeof(uintptr_t));
    *(--stack_ptr) = (1U << 24);                    ///<   Set thumb state bit in EPSR
    *(--stack_ptr) = (uintptr_t)task_exit_guard;    ///<   Set PC to task function handle
    *(--stack_ptr) = (uintptr_t)0xFFFFFFFDU;        ///<   Set LR to thread mode 
    *(--stack_ptr) = 0x0000000CU;                   ///<   Set R12 register default to its index
    *(--stack_ptr) = 0x00000003U;                   ///<   Set R3  register default to its index
    *(--stack_ptr) = 0x00000002U;                   ///<   Set R2  register default to its index
    *(--stack_ptr) = 0x00000001U;                   ///<   Set R1  register default to its index
    *(--stack_ptr) = 0x00000000U;                   ///<   Set R0  register to the argument for the tasks function
    __set_PSP(stack_ptr);
    __ISB(); // Ensure the stack pointer is updated before returning
}
