/**
* \brief This module contains the main function and etc
*/

/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "os.h"
#include "timers.h"
// Middleware
// Bios
// Standard C
#include <stdint.h>
/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Local Globals (not for libraries with application instantiation) ---- */
extern T_Scheduler JOCKTOSScheduler; ///<  Used for debugging (include in Watch List)
/* -- Private Function Declarations --------------------------------------- */

/* -- Public Functions----------------------------------------------------- */

/**
* \brief Place holder task
*
* changes some numbers in a predictable way
*
* \return
*/
void initial_task(uint32_t* new_sp) {
    int x = 0;
    int y = 100;
    while(1)
    {
        x++;
        if (x >= 100) x = 0;
        y--;
        if (y <= 0) y = 100;
    }
}
/**
 * @brief Burn clock cycles and occupy stack space
 * 
 * @param cycles O(cycles) complexity -> # of multiplications
 * @return int meaningless
 */
int burnCycles(int cycles) {
    int x;
    for (int i = 0; i < cycles; i++) {
        // Burn cycles by doing some arbitrary computation
        x = i * i;
    }
    return x;
}
/**
 * @brief Recursively occupy additional stack space.
 * 
 * For testing task stack overflow error handling
 * 
 * @param depth recursion depth
 * @param cycles multiplications between recursion calls and returns
 * @return int meaningless
 */
int inflateStack(int depth, int cycles) {
    int localVar = 0;  // This variable will occupy space on the stack

    if (depth > 0) {
        localVar = burnCycles(cycles);  // Burn cycles before making the recursive call
        localVar = inflateStack(depth - 1, cycles);  // Recursive call to inflate the stack further
    } 
    localVar = burnCycles(cycles);  // Burn cycles once the maximum depth is reached
    return localVar;
}
/**
* \brief Task function that intentionally causes a process stack overflow
*
* For testing task stack overflow error handling
*
* \return
*/
void intentionalStackOverflow(uint32_t* new_sp) {
    while(1) (void)inflateStack(900, 100);
}

int main(void)
{
    /**
     * Create 3 random TCBs to test linked list insertion
     */
    T_TaskControlBlock task1 = T_TASKCONTROLBLOCK_DEF(
        .u32StackSize_By=256, 
        .taskFunct=initial_task,
        .u8Name="task 1",
        .u8Priority=10);
    createTask(&task1);

    T_TaskControlBlock task2 = T_TASKCONTROLBLOCK_DEF(
        .u32StackSize_By=256, 
        .taskFunct=initial_task,
        .u8Name="task 2",
        .u8Priority=10);
    createTask(&task2);

    T_TaskControlBlock task3 = T_TASKCONTROLBLOCK_DEF(
        .u32StackSize_By=256, 
        .taskFunct=intentionalStackOverflow,
        .u8Name="task 3",
        .u8Priority=10);
    createTask(&task3);

    runJOCKTOS();

}
