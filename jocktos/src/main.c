/**
* \brief This module contains the main function and basic tasks
*/
#include "main.h"
#include "timers.h"
#include "synchro.h"
#include "stm32m4cortex_bsp.h"
#include <stdint.h>
/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */
/**
 * \brief Struct for testing purposes.
 *
 * It contains two integer values for testing the recursive stack inflation
 * task.
 */
typedef struct {
    int depth;
    int sleep_ms;
} TestArgStruct;

/* -- Local Globals (not for libraries with application instantiation) ---- */

extern Scheduler JOCKTOSScheduler; ///<  Used for debugging (include in Watch List)
Semaphore test_mutex = SEMAPHORE_DEF();

/* -- Functions----------------------------------------------------------- */

// Task function declarations
void sleepTest(void* arg);
void stackInflationTest(void* arg);
/**
 * @brief Basic main.c function that will initialize the scheduler, tasks, bitmap allocater, and 
 * deallocation of memory blocks.
 * 
 */
int main(void)
{
    /* Initialize the LED on the 'Nucleo' board*/
    jock_sys_LEDInit();
    /* Initialize Timer 2 on the board*/
    Timer2Init(TIM2, 3, 1000);

    // Configure kernel  to enable all builtin tasks
    JocktosConfig config = JOCKTOSCONFIG_DEF(
        .enable_idle = true,
        .enable_main = true,
        .enable_monitor = true,
        .allocator_block_size = 256
    );
    // Apply configuration to JOCKTOS kernel
    jock_os_configureJOCKTOS(&config);

    // Sample Sleep Task
    uint16_t sleep_ms = 1000;
    TaskControlBlock locking_sleep_task = TASKCONTROLBLOCK_DEF(
        .stack_size_bytes=512, 
        .task_handle=sleepTest,
        .task_arg=&sleep_ms,
        .name="sleep test");
    
    // Sample Stack usage Task
    TestArgStruct test_val = {.depth=10, .sleep_ms=1000};
    TaskControlBlock stack_usage_task = TASKCONTROLBLOCK_DEF(
        .stack_size_bytes=1024,
        .task_handle=stackInflationTest,
        .task_arg=&test_val,
        .name="stack inflation");
        
    jock_os_createTask(&locking_sleep_task); // Create Sleep Task in JOCKTOS
    jock_os_createTask(&stack_usage_task);   // Create Stack Usage Task in JOCKTOS
    jock_os_runJOCKTOS();                         // Start JOCKTOS Kernel

    // because enable_main is configured, execution **will** return here and continue
    int x = 100;
    int y = 0;
    // Infinite Loop with palce holder calculations for debugging
    while(1) {
        x++;
        if (x == 0) x = 100;
        y--;
        if (y == 100) y = 0;
    }
}

/**
 * @brief Task that will sleep for a specified amount of time
 * 
 * Task to test the sleep function, and competes with the stack inflation task
 * for a resource (test_mutex / semaphore).
 * 
 * @param arg Pointer to a uint16_t representing the amount of time to sleep
 */
void sleepTest(void* arg) {
    uint16_t sleep_time = *((uint16_t*)arg);
    while (true) {
        jock_synchro_takeSempahore(&test_mutex);
        jock_synchro_sleep(sleep_time);
        jock_synchro_giveSempahore(&test_mutex);
        jock_synchro_sleep(sleep_time);
    }
}

/**
* \brief Recursively occupy additional stack space.
*
* \param depth recursion depth
* \param sleep_ms amount of time to sleep
* \return meaningless, used to avoid compiler optimization and warnings
*/
int inflateStack(int depth, int sleep_ms) {
    int local_var = 0;  // This variable will occupy space on the stack

    if (depth > 0) {
        jock_synchro_takeSempahore(&test_mutex);
        jock_synchro_sleep(sleep_ms);
        jock_synchro_giveSempahore(&test_mutex);
        jock_synchro_sleep(sleep_ms);
        local_var += inflateStack(depth - 1, sleep_ms);  // Recursive call to inflate the stack further
    }
    return local_var;
}

/**
* \brief Task function that intentionally causes a process stack overflow
*
* For testing task stack overflow error handling
*
* @param arg Pointer to a TestArgStruct, used to specify recursion depth and cycles to burn
*/
void stackInflationTest(void* arg) {
    TestArgStruct* test_val = (TestArgStruct*)arg;
    while(1) (void)inflateStack(test_val->depth, test_val->sleep_ms);
}
