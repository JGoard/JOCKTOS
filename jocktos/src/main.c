/**
* \brief This module contains the main function and basic tasks
*/
#include "main.h"
#include "stm32f303xe.h"
#include <stdint.h>
/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Local Globals (not for libraries with application instantiation) ---- */

extern Scheduler JOCKTOSScheduler; ///<  Used for debugging (include in Watch List)
Semaphore test_mutex = SEMAPHORE_DEF();

/* -- Functions----------------------------------------------------------- */
/**
 * @brief   Basic main.c function that will initialize the scheduler, tasks, bitmap allocater, and *          deallocation of memory blocks.
 * 
 */
int main(void)
{
    jock_sys_LEDInit();
    /* Initialize the LED on the 'Nucleo' board*/

    /* Configuration Default for the Allocator and option for kernel servicing and monitoring */
    JocktosConfig config = JOCKTOSCONFIG_DEF(
        .enable_idle = true,
        .enable_main = true,
        .enable_monitor = true,
        .allocator_block_size = 256
    );
    /* Configures JOCKTOS Kernel with Default structure */
    jock_os_configureJOCKTOS(&config);
    
    /* Sample Task Employing passing in a task argument of anytype */
    TestArgStruct test_val = {.value = 1234, .id = "Test Val!\n"};

    TaskControlBlock testTask = TASKCONTROLBLOCK_DEF(
        .stack_size_bytes=512,
        .task_handle=testArgsTask,
        .task_arg=(void*)&test_val,
        .name="test args");
    jock_os_createTask(&testTask);

    /* Sample Sleep Task */
    TaskControlBlock sleepTask = TASKCONTROLBLOCK_DEF(
        .stack_size_bytes=512, 
        .task_handle=sleepTest,
        .name="sleep test");
    jock_os_createTask(&sleepTask);

    /* Sample Semaphore Task */
    TaskControlBlock lockTask = TASKCONTROLBLOCK_DEF(
        .stack_size_bytes=512,
        .task_handle=mutexTestTask,
        .name="semaphore test");
    jock_os_createTask(&lockTask);

    /* Sample Task Monitor Stack Test Task */
    TaskControlBlock stackTask = TASKCONTROLBLOCK_DEF(
        .stack_size_bytes=512,
        .task_handle=stackInflationTestTask,
        .name="stack inflation");
    jock_os_createTask(&stackTask);

    /* This will start the scheduler and tasking system */
    jock_os_runJOCKTOS();

    _testTimersInit();    

    int x = 100;
    int y = 0;
    while(1) {
        x++;
        if (x == 0) x = 100;
        y--;
        if (y == 100) y = 0;
    }
}

void testArgsTask(void* arg) {
    TestArgStruct* test_val = (TestArgStruct*)arg;
    int check = 0;
    while(1) {
        check++;
        if (check == test_val->value) {
            check = 0;
        }
    }
}

void sleepTest(void* arg) {
    while (true) {
        jock_os_takeSempahore(&test_mutex);
        jock_os_sleep(1000);
        jock_os_giveSempahore(&test_mutex);
        jock_os_sleep(1000);
    }
}

void mutexTestTask(void* arg) {
    uint32_t x = 10000;
    while(1) {
        x--;
        if (x == 5000) {
            jock_os_takeSempahore(&test_mutex);
        }
        if (x == 0) {
            jock_os_giveSempahore(&test_mutex);
            x = 10000;
        }
    }
}

int burnCycles(int cycles) {
    int x;
    for (int i = 0; i < cycles; i++) {
        // Burn cycles by doing some arbitrary computation
        x = i * i;
    }
    return x;
}

int inflateStack(int depth, int cycles) {
    int local_var = 0;  // This variable will occupy space on the stack

    if (depth > 0) {
        local_var = burnCycles(cycles);  // Burn cycles before making the recursive call
        local_var = inflateStack(depth - 1, cycles);  // Recursive call to inflate the stack further
    } 
    local_var = burnCycles(cycles);  // Burn cycles once the maximum depth is reached
    return local_var;
}

void stackInflationTestTask(void* arg) {
    while(1) (void)inflateStack(10, 100);
}
