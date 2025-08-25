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
#include "cmsis_gcc.h"
// Bios
// Standard C
#include <stdbool.h>
#include <stdlib.h>

/* -- Defines ------------------------------------------------------------- */

#define FILL        0xBABEFACE // for task allocation debugging
#define CANARY      0xDEADBEEF
#define CANARY_SIZE         64
#define CANARY_RASR_SIZE     5  // RASR size field: log2(64)=6 -> SIZE field = 6-1 = 5
#define HANDLER_STACK_SIZE 512
#define TRIGGER_PendSV *(uintptr_t volatile *)0xE000ED04 = (1U << 28)

/* -- Types --------------------------------------------------------------- */

uint8_t allocator_stack[ALLOCATOR_SIZE]         __attribute__((aligned(8))) = {0};
uint8_t os_handler_stack[HANDLER_STACK_SIZE]    __attribute__((aligned(8))) = {0};
/* -- Private Function Declarations --------------------------------------- */

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

void mpu_set_stack_guard(const uint32_t stack_base);

/**
* \brief pre defined OS task for idle.
*
* This function, idleJOCKTOS, enters an infinite loop where it continuously executes the __WFI instruction, 
* which puts the CPU into a low-power wait state, waiting for an interrupt to occur. 
* This function is used as the default idle task for the JOCKTOS kernel.
* //TODO: Figure out in the future a more aggressive power saving feature
* \param arg Unused
*/
void idleJOCKTOS(void* arg);

/**
 * \brief pre defined OS task to monitor stack usage
 */
void monitorJOCKTOS(void* arg);

/* -- Local Globals (not for libraries with application instantiation) ---- */

static Allocator allocator;
Scheduler JOCKTOSScheduler = {false, 0, NULL, NULL, NULL, NULL};
extern TCBError JOCKTOS_TCBError;

TaskControlBlock usr_main_tcb = TASKCONTROLBLOCK_DEF(
        .task_handle=NULL,
        .name="user space `main`");

TaskControlBlock stack_monitor_tcb = TASKCONTROLBLOCK_DEF(
        .stack_size_bytes=512, 
        .task_handle=monitorJOCKTOS,
        .name="stack usage monitor");

TaskControlBlock idle_tcb = TASKCONTROLBLOCK_DEF(
        .stack_size_bytes=512,
        .task_handle=idleJOCKTOS,
        .name="default OS idle task");

/* -- Public Functions----------------------------------------------------- */

void jock_os_install_task(TaskControlBlock* tcb) {
    // check if task function handle is valid
    if (!tcb->task_handle) {
        // TODO: better error handling
        JOCKTOS_TCBError.invalid_task_handle++;
        return;
    }
    tcb->stack_overflow = (uintptr_t*)_allocate(&allocator, tcb->stack_size_bytes);
    if (!tcb->stack_overflow) {
        // TODO: better error handling
        JOCKTOS_TCBError.failed_to_allocate++;
        return;
    }
    initializeStack(tcb);
    _insert_tcb(&JOCKTOSScheduler.ready, tcb);
}

void jock_os_switch_running_task(volatile TaskControlBlock** head) {
    // If there is a pending context switch, return immediately
    if (JOCKTOSScheduler.pending)
        return;
    // Set the pending flag to indicate that a context switch is pending
    JOCKTOSScheduler.pending = true;
    // If there is a currently running task
    if (JOCKTOSScheduler.running) {
        // Move the running task to the head of the ready list
        _insert_tcb(head, JOCKTOSScheduler.running);
    }
    // Iterate through the suspended tasks
    volatile TaskControlBlock* suspended = JOCKTOSScheduler.suspended;
    while (suspended != NULL) {
        // If the delay time for the task has been reached
        if (jock_os_get_time() >= suspended->delay_ms) {
            // Move the task from the suspended list to the ready list
            _move_tcb(&JOCKTOSScheduler.suspended, suspended, &JOCKTOSScheduler.ready);
            // Set the state of the task to ready
            suspended->state = READY;
        }
        // Move to the next task
        suspended = suspended->next;
    }
    TaskControlBlock* next_task = JOCKTOSScheduler.ready;
    if (next_task != &usr_main_tcb) mpu_set_stack_guard(next_task->stack_guard);
    // Trigger the PendSV interrupt to cause a context switch
    TRIGGER_PendSV;
}

void jock_os_configure(JocktosConfig* config) {
    // Initialize the allocator with the allocated memory and the block size specified in the config
    _init_allocator(&allocator, config->allocator_block_size, allocator_stack, ALLOCATOR_SIZE);
    // If the monitor is enabled, create a task for the stack usage monitor
    if (config->enable_monitor) jock_os_install_task(&stack_monitor_tcb);
    // If the main task is enabled, insert the userMainControlBlock into the JOCKTOSScheduler's running queue
    if (config->enable_main) _insert_tcb(&JOCKTOSScheduler.running, &usr_main_tcb);
    // If either the monitor or main task is not enabled, or both are not enabled, create a default OS idle task
    if (config->enable_idle || (!config->enable_monitor && !config->enable_main)) jock_os_install_task(&idle_tcb);
}

void jock_os_run(void) {
    uint32_t msp_top = os_handler_stack + sizeof(os_handler_stack) - 1;
    __set_PSP(__get_MSP());
    __set_CONTROL(__get_CONTROL() | (CONTROL_SPSEL_Msk));
    __ISB();
    __set_MSP(msp_top);
    __ISB();
    NVIC_SetPriority(PendSV_IRQn, 0xFFU);
    // Configure SysTick to generate an interrupt every 1 ms 
    SysTick_Configuration(127); ///<TODO: where tf does 127 come from...
    NVIC_SetPriority(SysTick_IRQn, 0U);
}
uint32_t jock_os_enter_critical_section(void){
    uint32_t  primask = 0;
    ///<TODO:Need to figure out why __get__PRIMASK crashes PENDSV Handler
    primask = __get_PRIMASK(); 
    __disable_irq();
    return primask;
}

void jock_os_leave_critical_section(uint32_t primask){
    if (primask == 0) {
        __enable_irq();
    }
}

/* -- Private Functions --------------------------------------------------- */

void initializeStack(TaskControlBlock* tcb) {
    uintptr_t* stack_ptr = tcb->stack_overflow;
    const uint32_t guard_base = (uint32_t)stack_ptr;
    tcb->stack_guard = (guard_base % CANARY_SIZE == 0) ?
                       (guard_base & ~(CANARY_SIZE - 1u)) : 
                       (guard_base + CANARY_SIZE - 1u) & ~(CANARY_SIZE - 1u);
    // set intermediate stack pointer to bottom of range
    stack_ptr = (uintptr_t*)(stack_ptr + tcb->stack_size_bytes / sizeof(uintptr_t));
    // define tasks initial exception return stack
    uintptr_t* init_stack_ptr;
    //  - non-critical registers are initialized to their index
    *(--stack_ptr) = (1U << 24);                    ///<   Set thumb state bit in EPSR
    *(--stack_ptr) = (uintptr_t)tcb->task_handle;   ///<   Set PC to task function handle
    *(--stack_ptr) = (uintptr_t)task_exit_guard;    // LR: Return trap
    *(--stack_ptr) = 0x0000000CU;                   ///<   Set R12 register default to its index
    *(--stack_ptr) = 0x00000003U;                   ///<   Set R3  register default to its index
    *(--stack_ptr) = 0x00000002U;                   ///<   Set R2  register default to its index
    *(--stack_ptr) = 0x00000001U;                   ///<   Set R1  register default to its index
    *(--stack_ptr) = (uintptr_t)tcb->task_arg;      ///<   Set R0  register to the argument for the tasks function
    init_stack_ptr = stack_ptr - 1;                 ///<   Catch top of initial post-exception stack
    *(--stack_ptr) = 0x0000000BU;                   ///<   Set R11 register default to its index
    *(--stack_ptr) = 0x0000000AU;                   ///<   Set R10 register default to its index
    *(--stack_ptr) = 0x00000009U;                   ///<   Set R9  register default to its index
    *(--stack_ptr) = 0x00000008U;                   ///<   Set R8  register default to its index
    *(--stack_ptr) = 0x00000007U;                   ///<   Set R7  register default to its index
    *(--stack_ptr) = 0x00000006U;                   ///<   Set R6  register default to its index
    *(--stack_ptr) = 0x00000005U;                   ///<   Set R5  register default to its index
    *(--stack_ptr) = 0x00000004U;                   ///<   Set R4  register default to its index
    tcb->stack_pointer = stack_ptr;
    // Fill unused process stack with known value
    while (stack_ptr > tcb->stack_overflow + CANARY_SIZE) {
        *(--stack_ptr) = FILL;
    }
    while (stack_ptr > tcb->stack_overflow) {
        *(--stack_ptr) = CANARY; ///< set top 8 bytes to something else
    }
}

// Align down to 32 bytes 
void mpu_set_stack_guard(const uint32_t stack_base) {
    MPU->RBAR = stack_base
              | MPU_RBAR_VALID_Msk
              | (7u << MPU_RBAR_REGION_Pos);
    // RASR size field: log2(64)=6 -> SIZE field = 6-1 = 5
    MPU->RASR = MPU_RASR_ENABLE_Msk
              | MPU_RASR_XN_Msk
              | (0u << MPU_RASR_AP_Pos)    // no access
              | (CANARY_RASR_SIZE << MPU_RASR_SIZE_Pos);

    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
    MPU->CTRL = MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_ENABLE_Msk;
    __DSB(); __ISB();
}

void monitorJOCKTOS(void* arg) {
    volatile TaskControlBlock* head = NULL;       // Pointer to the current task
    TaskState monitor_scope = RUNNING;            // Keeps track of which list of tasks to monitor next
    while(true) {                                   // Loop indefinitely

        switch (monitor_scope) {                     // Switch on the current list to monitor
            case READY: {                          // If the ready list is being monitored
                head = JOCKTOSScheduler.ready;      // Set the head pointer to the ready list
                monitor_scope = RUNNING;            // Set the monitor_scope to eRUNNING
                break;                              // Break out of the switch statement
            }   
            case RUNNING: {                        // If the running list is being monitored
                head = JOCKTOSScheduler.running;    // Set the head pointer to the running list
                monitor_scope = SUSPENDED;          // Set the monitor_scope to eSUSPENDED
                break;                              // Break out of the switch statement
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


void idleJOCKTOS(void* arg __attribute__((unused))) {
  /* Disable interrupts to make sure that the busy flag does not get 
     modified between the check in the while condition and the system
     sleep */
    uint32_t irq_flag = UINT32_MAX;

    while (1) {
    //   /*
    //    * Disable interrupts to make sure that the busy flag does not get
    //    * modified between the check in the while condition and the system
    //    * sleep.
    //    */
        irq_flag = jock_os_enter_critical_section();

    //   // Check if the busy flag has been set

      if (irq_flag == 0) {
    //     /* 
    //     * __DSB stands for Data Synchronization Barrier. It ensures that all memory
    //     * accesses before it are completed before any memory accesses after it start.
    //     * Here, we use __DSB to ensure that all memory accesses before it are
    //     * completed before any memory accesses after it start.
    //     */
        // __DSB();

    //     /*
    //     *__WFI stands for Wait For Interrupt. It is an ARM instruction that puts the processor
    //     into a low-power state, where it waits for an interrupt to occur before waking up and
    //     executing the interrupt handler. This is useful for power saving, as it consumes very
    //     little power when the processor is in this state.
    //     */
        __WFI();
      }

    //     // Enable interrupts again
        jock_os_leave_critical_section(irq_flag);

    // // If the busy flag has been set, exit the loop
      if (irq_flag != 0) {
        break;
      }

    // /*
    //  * __ISB stands for Instruction Synchronization Barrier.
    //  * It is an ARM instruction that flushes the pipeline in the processor,
    //  * ensuring that all instructions before it are completed before any
    //  * instructions after it are started.
    //  *
    //  * We use __ISB to flush the pipeline in the processor, so that all
    //  * instructions before it are completed before any instructions after
    //  * it are started.
    //  */
      __ISB();      ///<TODO: Is this necessary?
    }
    // TODO: Figure out how to low power
}

__attribute__((noreturn))
void task_exit_guard(void) {
    uint32_t primask = jock_os_enter_critical_section();
    JOCKTOSScheduler.running->state = TERMINATED;
    jock_os_switch_running_task(&JOCKTOSScheduler.terminated);
    jock_os_leave_critical_section(primask);
}
