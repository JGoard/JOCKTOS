/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "os.h"
#include "tcb.h"
#include "synchro.h"
// Middleware
// Bios
// Standard C

/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Private Function Declarations --------------------------------------- */

/**
 * @brief Task function for updating task metrics.
 * 
 * @param arg [unused]
 */
void monitorJOCKTOS(void* arg __attribute__((unused)));

/**
 * @brief Default task function for JOCKTOS idle state
 * 
 * @param arg [unused]
 */
void idleJOCKTOS(void* arg __attribute__((unused)));

/* -- Local Globals (not for libraries with application instantiation) ---- */

extern Scheduler JOCKTOSScheduler;

TaskControlBlock usr_main_tcb = TASKCONTROLBLOCK_DEF(
        .task_handle=NULL); // "__main__"

TaskControlBlock stack_monitor_tcb = TASKCONTROLBLOCK_DEF(
        .stack_size_bytes=256, 
        .task_handle=monitorJOCKTOS); // "monitor_"

TaskControlBlock idle_tcb = TASKCONTROLBLOCK_DEF(
        .stack_size_bytes=256,
        .task_handle=idleJOCKTOS); // "_idle_OS_"

/* -- Public Functions----------------------------------------------------- */

/* -- Private Functions --------------------------------------------------- */

/**
 * @details This function is used to update the stack usage for each active task.
 *          This value is used for internal metrics and logging.
 *          Memory management and overflow protection is handled independently.
 */
void monitorJOCKTOS(void* arg __attribute__((unused))) {
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
            case SUSPENDED: {                      // If the suspended list is being monitored
                head = JOCKTOSScheduler.suspended;  // Set the head pointer to the suspended list
                monitor_scope = TERMINATED;         // Set the monitor_scope to eTERMINATED
                break;                              // Break out of the switch statement
            }
            default: {
                jock_synchro_sleep(1000);          // Sleep for 1000 milliseconds
                monitor_scope = READY;              // Set the monitor_scope to eREADY
                head = NULL;                        // Set the head pointer to NULL
                break;                              // Break out of the switch statement
            }
        }
        while(head != NULL) {
            monitorStackUsage(&head);
            head = head->next;
        }
    }
}

/**
 * @details This function is used as the default idle task for the OS. 
 *          It enters a low power infinite loop where it waits for an interrupt to occur.
 */
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
