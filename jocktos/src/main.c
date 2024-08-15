/**
* \brief This module contains the main function and basic tasks
*/
#include "main.h"
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
void stackInflationTestTask(void* arg);

/**
 * @brief Basic main.c function that will initialize the scheduler, tasks, bitmap allocater, and 
 * deallocation of memory blocks.
 * 
 */
int main(void)
{
    // Configure kernel  to enable all builtin tasks
    JocktosConfig config = JOCKTOSCONFIG_DEF(
        .enable_idle = true,
        .enable_main = true,
        .enable_monitor = true,
        .allocator_block_size = 256
    );
    // Apply configuration to JOCKTOS kernel
    jock_configure(&config);

    // Sample Sleep Task
    uint16_t sleep_ms = 1000;
    TaskControlBlock lockingSleepTask = TASKCONTROLBLOCK_DEF(
        .stack_size_bytes=512, 
        .task_handle=sleepTest,
        .task_arg=&sleep_ms,
        .name="sleep test");
    
    // Sample Stack usage Task
    TestArgStruct test_val = {.depth=10, .sleep_ms=1000};
    TaskControlBlock stackUsageTask = TASKCONTROLBLOCK_DEF(
        .stack_size_bytes=1024,
        .task_handle=stackInflationTestTask,
        .task_arg=&test_val,
        .name="stack inflation");
        
    jock_createTask(&lockingSleepTask); // Create Sleep Task in JOCKTOS
    jock_createTask(&stackUsageTask);   // Create Stack Usage Task in JOCKTOS
    jock_run();                         // Start JOCKTOS Kernel

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
        jock_takeSemaphore(&test_mutex);
        jock_sleep(sleep_time);
        jock_giveSemaphore(&test_mutex);
        jock_sleep(sleep_time);
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
        jock_takeSemaphore(&test_mutex);
        jock_sleep(sleep_ms);
        jock_giveSemaphore(&test_mutex);
        jock_sleep(sleep_ms);
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
void stackInflationTestTask(void* arg) {
    TestArgStruct* test_val = (TestArgStruct*)arg;
    while(1) (void)inflateStack(test_val->depth, test_val->sleep_ms);
}
