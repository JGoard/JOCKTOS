
main.elf:     file format elf32-littlearm


Disassembly of section .text:

00000000 <VectorTable>:
   0:	00 00 01 20 23 00 00 00                             ... #...

00000008 <DoWait>:

#include <common.h>


static void DoWait(void)
{
   8:	b082      	sub	sp, #8
    volatile int i;

    /* wait ~0.5s */
    for(i=0; i<0x100000; i++);
   a:	2300      	movs	r3, #0
   c:	9301      	str	r3, [sp, #4]
   e:	e002      	b.n	16 <DoWait+0xe>
  10:	9b01      	ldr	r3, [sp, #4]
  12:	3301      	adds	r3, #1
  14:	9301      	str	r3, [sp, #4]
  16:	9b01      	ldr	r3, [sp, #4]
  18:	f5b3 1f80 	cmp.w	r3, #1048576	; 0x100000
  1c:	dbf8      	blt.n	10 <DoWait+0x8>
}
  1e:	b002      	add	sp, #8
  20:	4770      	bx	lr

00000022 <OnReset>:

/* reset function called by CPU upon start */
void OnReset(void)
{
  22:	b508      	push	{r3, lr}
    /* Perform endless blinking loop */
    while(1)
    {
        /* Use this wait function to illustrate
         * different C-compiler optimizations */
        DoWait();
  24:	f7ff fff0 	bl	8 <DoWait>
    while(1)
  28:	e7fc      	b.n	24 <OnReset+0x2>
