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

#define TRIGGER_PendSV *(uintptr_t volatile *)0xE000ED04 = (1U << 28)

/* -- Types --------------------------------------------------------------- */

/* -- Private Function Declarations --------------------------------------- */

/**
 * \brief Updates the task control blocks stackUsage
 * 
 * \param tcb Pointer to task control block to be monitored.
 */
static inline void monitorStackUsage(volatile T_TaskControlBlock** tcb) {
    (*tcb)->stackUsage = 100.0 * (1.0 - ((double)((*tcb)->u32TaskStackPointer \
    - (*tcb)->u32TaskStackOverflow)) / (double)((*tcb)->u32StackSize_By * sizeof(uintptr_t)));
}

/**
 * @brief Initializes the stack for a task in a real-time operating system (RTOS).
 *
 * This function sets up the initial stack frame for the task by populating the stack with default values for various registers.
 * It also sets the program counter (PC) to the task function handle, the link register (LR) to a specific value, and the stack pointer (SP) to the bottom of the stack range.
 * After setting up the initial stack frame, the function fills the remaining unused stack space with a known value and sets the top 8 bytes to another known value.
 *
 * @param tcb Pointer to a T_TaskControlBlock structure representing the task.
 */
void initializeStack(T_TaskControlBlock* tcb);

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

void configureJOCKTOS(T_JocktosConfig* config) {
    // Allocate memory for the allocator
    void* memory = calloc(ALLOCATOR_SIZE, 1);
    // Initialize the allocator with the allocated memory and the block size specified in the config
    initAllocator(&allocator, memory, ALLOCATOR_SIZE, config->allocatorBlockSize);
    // If the monitor is enabled, create a task for the stack usage monitor
    if (config->enableMonitor) createTask(&stackUsageMonitor);
    // If the main task is enabled, insert the userMainControlBlock into the JOCKTOSScheduler's running queue
    if (config->enableMain) insertTCB(&JOCKTOSScheduler.running, &userMainControlBlock);
    // If either the monitor or main task is not enabled, or both are not enabled, create a default OS idle task
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
    SysTick_Configuration(127); ///<TODO: where tf does 127 come from...
    NVIC_SetPriority(SysTick_IRQn, 0U);
}

/* -- Private Functions --------------------------------------------------- */

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
    tcb->u32TaskStackPointer = taskStack;
    // Fill unused process stack with known value
    while (taskStack > tcb->u32TaskStackOverflow + 8) {
        *(--taskStack) = 0xBABEFACEU;
    }
    while (taskStack > tcb->u32TaskStackOverflow) {
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


void monitorJOCKTOS(void* arg) {
    volatile T_TaskControlBlock* head = NULL;       // Pointer to the current task
    E_TaskState monitorScope = eRUNNING;            // Keeps track of which list of tasks to monitor next
    while(true) {                                   // Loop indefinitely

        switch (monitorScope) {                     // Switch on the current list to monitor
            case eREADY: {                          // If the ready list is being monitored
                head = JOCKTOSScheduler.ready;      // Set the head pointer to the ready list
                monitorScope = eRUNNING;            // Set the monitorScope to eRUNNING
                break;                              // Break out of the switch statement
            }   
            case eRUNNING: {                        // If the running list is being monitored
                head = JOCKTOSScheduler.running;    // Set the head pointer to the running list
                monitorScope = eSUSPENDED;          // Set the monitorScope to eSUSPENDED
                break;                              // Break out of the switch statement
            }
            default: {                              // If the suspended list is being monitored
                head = JOCKTOSScheduler.suspended;  // Set the head pointer to the suspended list
                monitorScope = eREADY;              // Set the monitorScope to eREADY
                break;                              // Break out of the switch statement
            }
        }
        while(head != NULL) {                       // Loop through all tasks in the current list
            monitorStackUsage(&head);               // Monitor the stack usage of the current task
            head = head->TCBNext;                   // Move to the next task in the list
        }
    }
}

volatile bool irq_flag;

void idleJOCKTOS(void* arg) {
int was_masked;

  /* Disable interrupts to make sure that the busy flag does not get 
     modified between the check in the while condition and the system
     sleep */
    volatile bool irq_flag;
  
  while (!irq_flag)
  {
    while (1) {
      // Disable interrupts to make sure that the busy flag does not get
      // modified between the check in the while condition and the system
      // sleep
      __disable_irq();

      // Check if the busy flag has been set
      bool done = irq_flag;

      // If the busy flag has been set, exit the loop
      if (!done) {
        // __DSB stands for Data Synchronization Barrier. It is an ARM
        // instruction that ensures that all memory accesses before it are
        // completed before any memory accesses after it start.
        // Here, we use __DSB to ensure that all memory accesses before it
        // are completed before any memory accesses after it start.

        __DSB();
        /*
        __WFI stands for Wait For Interrupt. It is an ARM instruction that puts the processor
        into a low-power state, where it waits for an interrupt to occur before waking up and
        executing the interrupt handler. This is useful for power saving, as it consumes very
        little power when the processor is in this state.
        */
        __WFI();
      }

      // Enable interrupts again
      __enable_irq();

      // If the busy flag has been set, exit the loop
      if (done) {
        break;
      }

    // __ISB stands for Instruction Synchronization Barrier. It is an ARM
    // instruction that flushes the pipeline in the processor, so that all
    // instructions before it are completed before any instructions after
    // it are started.
    // We use __ISB to flush the pipeline in the processor, so that all
    // instructions before it are completed before any instructions after
    // it are started.
      __ISB();
    }
    // TODO: Figure out how to low power
    }
}