/**
* \brief This module contains the main function and etc
*/

/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include <stdint.h>
#include "os.h"
#include "timers.h"

// Middleware
// Bios
// Standard C

/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Local Globals (not for libraries with application instantiation) ---- */

/* -- Private Function Declarations --------------------------------------- */

/* -- Public Functions----------------------------------------------------- */

/**
* \brief Define function...
*
* Lorem Ipsum
*
* \return
*/

extern T_Scheduler JOCKTOSScheduler;

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
int swamp = 0;
int main(void)
{
    T_TaskControlBlock task1 = T_TASKCONTROLBLOCK_DEF(
        .u32StackSize_By=1024, 
        .taskFunct=initial_task,
        .u8Name="task 1",
        .u8Priority=10);
    _Bool task1_create = createTask(&task1);

    T_TaskControlBlock task2 = T_TASKCONTROLBLOCK_DEF(
        .u32StackSize_By=1024, 
        .taskFunct=initial_task,
        .u8Name="task 2",
        .u8Priority=2);
    _Bool task2_create = createTask(&task2);

    T_TaskControlBlock task3 = T_TASKCONTROLBLOCK_DEF(
        .u32StackSize_By=1024, 
        .taskFunct=initial_task,
        .u8Name="task 3",
        .u8Priority=3);
    _Bool task3_create = createTask(&task3);

    
    for(;;){
        __asm volatile ("PUSH {R0-R11}");
        swamp++;
        __asm volatile ("POP {R0-R11}");
    }

}
