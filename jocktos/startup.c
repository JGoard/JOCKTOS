#include <stdint.h>
#include "main.h"

extern const uint32_t StackTop;

/* declare vector table */
__attribute__ ((section(".vtor")))
const void* VectorTable[] = {
    &StackTop,
    JocktosMain,
};
