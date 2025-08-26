#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t memreg_t;

#pragma pack(push, 1)
typedef struct {
    memreg_t r0;
    memreg_t r1;
    memreg_t r2;
    memreg_t r3;
    memreg_t r12;
    memreg_t lr;
    memreg_t pc;
    memreg_t xpsr;
} ExceptionFrame;

typedef struct {
    memreg_t r4;
    memreg_t r5;
    memreg_t r6;
    memreg_t r7;
    memreg_t r8;
    memreg_t r9;
    memreg_t r10;
    memreg_t r11;
} RegisterCache;

typedef struct {
    RegisterCache registers;
    ExceptionFrame exc_frame;
} FullContextFrame;
#pragma pack(pop)

#define EXCEPTION_FRAME_INIT(task_fn, arg, ret_reg) {   \
    .r0  = (memreg_t)(arg),                           \
    .r1  = 0x00000001U,                                 \
    .r2  = 0x00000002U,                                 \
    .r3  = 0x00000003U,                                 \
    .r12 = 0x0000000CU,                                 \
    .lr  = (memreg_t)(ret_reg),                       \
    .pc  = (memreg_t)(task_fn),                       \
    .xpsr = (1U << 24) /* Thumb state */                \
}

#define REGISTER_CACHE_INIT {   \
    .r4  = 0x00000004U,         \
    .r5  = 0x00000005U,         \
    .r6  = 0x00000006U,         \
    .r7  = 0x00000007U,         \
    .r8  = 0x00000008U,         \
    .r9  = 0x00000009U,         \
    .r10 = 0x0000000AU,         \
    .r11 = 0x0000000BU          \
}

#define FULL_CONTEXT_FRAME_INIT(task_fn, arg, ret_reg) {        \
    .registers = REGISTER_CACHE_INIT,                           \
    .exc_frame = EXCEPTION_FRAME_INIT(task_fn, arg, ret_reg)    \
}

