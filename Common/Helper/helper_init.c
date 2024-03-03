/**************************************************************************//**
 * @file     helper_init.c
 * @brief    Perform C-compliant memory initializations
 * @date     
 ******************************************************************************/

#include <common.h>
#include <helper.h>

/*
 * The following are constructs created by the linker, indicating where the
 * the "data" and "bss" segments reside in memory.  The initializers for the
 * for the "data" segment resides immediately following the "text" segment.
 */
extern unsigned long __end_of_text__;
extern unsigned long __data_beg__;
extern unsigned long __data_end__;
extern unsigned long __bss_beg__;
extern unsigned long __bss_end__;

/*
 * This is the code that gets called when the processor first starts execution
 * following a reset event.  Only the absolutely necessary set is performed,
 * after which the application supplied entry() routine is called.  Any fancy
 * actions (such as making decisions based on the reset cause register, and
 * resetting the bits in that register) are left solely in the hands of the
 * application.
 */
void OnReset(void)
{
    uint32_t *src, *dst;

    /*
     * Copy the data segment initializers from flash to SRAM.
     */
    src = &__end_of_text__;
    for(dst = &__data_beg__; dst < &__data_end__;)
        *dst++ = *src++;

    /*
     * Zero fill the bss segment.  This is done with inline assembly
     * since this will clear the value of pulDest if it is not kept in
     * a register.
     */
    for(dst = &__bss_beg__; dst < &__bss_end__;)
        *dst++ = 0;

    /*
     * Call user firmware entry
     */
    main_entry();

    /*
     * main() shouldn't return, but if it does, we'll just enter an infinite loop.
     */
    while(true);
}
