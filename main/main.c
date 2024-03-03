/**************************************************************************//**
 * @file     main.c
 * @brief    Demonstrate minimum program
 * @date     
 ******************************************************************************/

#include <common.h>


static void DoWait(void)
{
    volatile int i;

    /* wait ~0.5s */
    for(i=0; i<0x100000; i++);
}

/* reset function called by CPU upon start */
void OnReset(void)
{
    /* Perform endless blinking loop */
    while(1)
    {
        /* Use this wait function to illustrate
         * different C-compiler optimizations */
        DoWait();
    };
}

/* stack declared in ld */
extern const uint32_t StackTop;

/* declare vector table */
__attribute__ ((section(".vtor")))
const void* VectorTable[] = {
    &StackTop,   /* CPU will automatically       *
                  * set stack its pointer        *
                  * to this value                */

    OnReset,     /* -15: Reset_IRQn              */
};

