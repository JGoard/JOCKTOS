/**
* \brief This module is the core JOCKTOS kernel functionality
*/
/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "errors.h"
#include "os.h"
#include "tcb.h"
#include "timers.h"
#include "context_frame.h"
// Middleware
#include "bsp.h"
#include "core_cm4.h"
#include "cmsis_gcc.h"
#include "block_allocator.h"
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
#define ALLOCATOR_SIZE    8192
#define TRIGGER_PendSV *(uintptr_t volatile *)0xE000ED04 = (1U << 28)

/* -- Types --------------------------------------------------------------- */

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

uint8_t allocator_stack[ALLOCATOR_SIZE]         __attribute__((aligned(8))) = {0};
uint8_t os_handler_stack[HANDLER_STACK_SIZE]    __attribute__((aligned(8))) = {0};

Scheduler JOCKTOSScheduler = {false, 0, NULL, NULL, NULL, NULL};

static BlockAllocator allocator;
Buffer* JOCKTOS_log_buffer;

extern TCBError JOCKTOS_TCBError;
extern TaskControlBlock usr_main_tcb;
extern TaskControlBlock stack_monitor_tcb;
extern TaskControlBlock idle_tcb;
/* -- Public Functions----------------------------------------------------- */

void jock_os_install_task(TaskControlBlock* tcb) {
    // check if task function handle is valid
    if (!tcb->task_handle) {
        jock_os_log(&(JOCKTOSMessage) {
            .type = ERROR,
            .code = FAILED_TO_INSTALL_TASK,
            .tick_count = jock_os_get_time(),
            .task_name = tcb->name.as_int
        });
        return;
    }
    tcb->stack_overflow = (uintptr_t*)blockAllocate(&allocator, tcb->stack_size_bytes);
    if (!tcb->stack_overflow) {
        jock_os_log(&(JOCKTOSMessage) {
            .type = ERROR,
            .code = ALLOCATION_FAILED,
            .tick_count = jock_os_get_time(),
            .task_name = tcb->name.as_int
        });
        return;
    }
    initializeStack(tcb);
    _insert_tcb(&JOCKTOSScheduler.ready, tcb);

    jock_os_log(&(JOCKTOSMessage) {
        .type = INFO,
        .code = TASK_CREATED,
        .tick_count = jock_os_get_time(),
        .task_name = tcb->name.as_int
    });
}

void jock_os_switch_running_task(volatile TaskControlBlock** head) {
    // If there is a pending context switch, return immediately
    if (JOCKTOSScheduler.pending) {
        jock_os_log(&(JOCKTOSMessage) {
            .type = WARNING,
            .code = CONTEXT_SWITCH_PENDING,
            .tick_count = jock_os_get_time(),
            .task_name = JOCKTOSScheduler.running->name.as_int
        });
        return;
    }
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
    initBlockAllocator(&allocator, config->allocator_block_size, allocator_stack, ALLOCATOR_SIZE);
    JOCKTOS_log_buffer = bufferAllocate(&allocator, config->logger_size, sizeof(JOCKTOSMessage));
    if (!JOCKTOS_log_buffer) {
        // TODO: how to handle log buffer allocation failure?
    }
    // Always install the idle task
    idle_tcb.name.as_int = TASK_NAME("_idle_OS_");
    jock_os_install_task(&idle_tcb);
    // If the monitor is enabled, create a task for the stack usage monitor
    if (config->enable_monitor) {
        stack_monitor_tcb.name.as_int = TASK_NAME("monitor_");
        jock_os_install_task(&stack_monitor_tcb);
    }
    // If the main task is enabled, insert the userMainControlBlock into the JOCKTOSScheduler's running queue
    if (config->enable_main) {
        usr_main_tcb.name.as_int = TASK_NAME("__main__");
        _insert_tcb(&JOCKTOSScheduler.running, &usr_main_tcb);
        jock_os_log(&(JOCKTOSMessage) {
            .type = INFO,
            .code = TASK_CREATED,
            .task_name = usr_main_tcb.name.as_int,
            .tick_count = jock_os_get_time()
        });
    }
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

void jock_os_leave_critical_section(uint32_t primask) {
    if (primask == 0) {
        __enable_irq();
    }
}

__attribute__((noreturn))
void jock_os_kill_task(void) {
    uint32_t primask = jock_os_enter_critical_section();
    JOCKTOSScheduler.running->state = TERMINATED;
    jock_os_log(&(JOCKTOSMessage) {
        .type = ERROR,
        .code = TASK_TERMINATED,
        .task_name = JOCKTOSScheduler.running->name.as_int,
        .tick_count = jock_os_get_time()
    });
    jock_os_switch_running_task(&JOCKTOSScheduler.terminated);
    jock_os_leave_critical_section(primask);
}

/* -- Private Functions --------------------------------------------------- */

void initializeStack(TaskControlBlock* tcb) {
    uint8_t* stack_ptr = (uint8_t*)tcb->stack_overflow;
    const uint32_t guard_base = (uint32_t)stack_ptr;
    tcb->stack_guard = (guard_base % CANARY_SIZE == 0) ?
                       (guard_base & ~(CANARY_SIZE - 1u)) : 
                       (guard_base + CANARY_SIZE - 1u) & ~(CANARY_SIZE - 1u);
    // set intermediate stack pointer to bottom of range
    stack_ptr += tcb->stack_size_bytes;
    FullContextFrame* frame = (FullContextFrame*)(stack_ptr - sizeof(FullContextFrame));
    *frame = (FullContextFrame)FULL_CONTEXT_FRAME_INIT(
        tcb->task_handle,       // PC / function to fall into
        tcb->task_arg,          // R0 / argument for function
        jock_os_kill_task         // LR / return address
    );
    tcb->stack_pointer = frame;
    stack_ptr = (uint8_t*)frame;
    // Fill unused process stack with known value
    while (stack_ptr > tcb->stack_overflow + CANARY_SIZE) {
        stack_ptr -= sizeof(sysregister_t);
        *(sysregister_t*)stack_ptr = FILL;
    }
    while (stack_ptr > tcb->stack_overflow) {
        stack_ptr -= sizeof(sysregister_t);
        *(sysregister_t*)stack_ptr = CANARY;
    }
}

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
