/**
* \brief This module contains the main function and basic tasks
*/
#include "main.h"
#include <stdint.h>
/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Local Globals (not for libraries with application instantiation) ---- */

extern T_Scheduler JOCKTOSScheduler; ///<  Used for debugging (include in Watch List)
T_Semaphore testMutex = T_SEMAPHORE_DEF();

/* -- Functions----------------------------------------------------------- */

int main(void)
{
    T_JocktosConfig config = T_JOCKTOSCONFIG_DEF(
        .enableIdle = true,
        .enableMain = true,
        .enableMonitor = true,
        .allocatorBlockSize = 128
    );
    configureJOCKTOS(&config);

    T_TaskControlBlock sleepTask = T_TASKCONTROLBLOCK_DEF(
        .u32StackSize_By=256, 
        .taskFunct=sleepTest,
        .u8Name="sleep test");
    createTask(&sleepTask);

    T_TaskControlBlock lockTask = T_TASKCONTROLBLOCK_DEF(
        .u32StackSize_By=256, 
        .taskFunct=mutexTestTask,
        .u8Name="semaphore test");
    createTask(&lockTask);

    T_TaskControlBlock stackTask = T_TASKCONTROLBLOCK_DEF(
        .u32StackSize_By=256, 
        .taskFunct=stackInflationTestTask,
        .u8Name="stack inflation");
    createTask(&stackTask);

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

void sleepTest(uintptr_t* new_sp) {
    while (true) {
        takeSemaphore(&testMutex);
        sleep(1000);
        giveSemaphore(&testMutex);
        sleep(1000);
    }
}

void mutexTestTask(uintptr_t* new_sp) {
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

void stackInflationTestTask(uintptr_t* new_sp) {
    while(1) (void)inflateStack(10, 100);
}