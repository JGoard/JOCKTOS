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
        .taskFunct=initial_task,
        .u8Name="task 3",
        .u8Priority=10);
    createTask(&task3);

    runJOCKTOS();

}
