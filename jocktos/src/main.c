/**
* \brief This module contains the main function and basic tasks
*/
#include "main.h"
#include <stdint.h>
/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Local Globals (not for libraries with application instantiation) ---- */

extern Scheduler JOCKTOSScheduler; ///<  Used for debugging (include in Watch List)
Semaphore testMutex = SEMAPHORE_DEF();

/* -- Functions----------------------------------------------------------- */
/**
 * @brief   Basic main.c function that will initialize the scheduler, tasks, bitmap allocater, and *          deallocation of memory blocks.
 * 
 */
int main(void)
{
    /* Configuration Default for the Allocator and option for kernel servicing and monitoring */
    JocktosConfig config = JOCKTOSCONFIG_DEF(
        .enableIdle = true,
        .enableMain = true,
        .enableMonitor = true,
        .allocatorBlockSize = 256
    );
    /* Configures JOCKTOS Kernel with Default structure */
    configureJOCKTOS(&config);
    
    /* Sample Task Employing passing in a task argument of anytype */
    TestArgStruct test_val = {.value = 1234, .ID = "Test Val!\n"};

    TaskControlBlock testTask = TASKCONTROLBLOCK_DEF(
        .u32StackSize_By=512,
        .taskFunct=testArgsTask,
        .taskArg=(void*)&test_val,
        .name="test args");
    createTask(&testTask);

    /* Sample Sleep Task */
    TaskControlBlock sleepTask = TASKCONTROLBLOCK_DEF(
        .u32StackSize_By=512, 
        .taskFunct=sleepTest,
        .name="sleep test");
    createTask(&sleepTask);

    /* Sample Semaphore Task */
    TaskControlBlock lockTask = TASKCONTROLBLOCK_DEF(
        .stackSize_By=512,
        .taskFunct=mutexTestTask,
        .name="semaphore test");
    createTask(&lockTask);

    /* Sample Task Monitor Stack Test Task */
    TaskControlBlock stackTask = TASKCONTROLBLOCK_DEF(
        .stackSize_By=512,
        .taskFunct=stackInflationTestTask,
        .name="stack inflation");
    createTask(&stackTask);

    /* This will start the scheduler and tasking system */
    runJOCKTOS();

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
        takeSemaphore(&testMutex);
        sleep(1000);
        giveSemaphore(&testMutex);
        sleep(1000);
    }
}

void mutexTestTask(void* arg) {
    uint32_t x = 10000;
    while(1) {
        x--;
        if (x == 5000) {
            takeSemaphore(&testMutex);
        }
        if (x == 0) {
            giveSemaphore(&testMutex);
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
    int localVar = 0;  // This variable will occupy space on the stack

    if (depth > 0) {
        localVar = burnCycles(cycles);  // Burn cycles before making the recursive call
        localVar = inflateStack(depth - 1, cycles);  // Recursive call to inflate the stack further
    } 
    localVar = burnCycles(cycles);  // Burn cycles once the maximum depth is reached
    return localVar;
}

void stackInflationTestTask(void* arg) {
    while(1) (void)inflateStack(10, 100);
}