#pragma once
/* -- Includes ------------------------------------------------------------ */
// Jocktos
// Middleware
// Bios
// Standard C
#include <stdint.h>
#include <stdbool.h>

/* -- Defines ------------------------------------------------------------- */

/**
 * @brief Initializes all exception frame registers to their index
 * 
 * @param task_fn Pointer to the task function
 * @param arg Argument to the task function
 * @param ret_reg Return register
 * 
 */
#define EXCEPTION_FRAME_INIT(task_fn, arg, ret_reg) {   \
    .r0  = (sysregister_t)(arg),                           \
    .r1  = 0x00000001U,                                 \
    .r2  = 0x00000002U,                                 \
    .r3  = 0x00000003U,                                 \
    .r12 = 0x0000000CU,                                 \
    .lr  = (sysregister_t)(ret_reg),                       \
    .pc  = (sysregister_t)(task_fn),                       \
    .xpsr = (1U << 24) /* Thumb state */                \
}

/**
 * @brief Initializes all additional registers to their index
 * 
 */
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

/**
 * @brief Initializes all context frame registers to their index
 * 
 * @param task_fn Pointer to the task function
 * @param arg Argument to the task function
 * @param ret_reg Return register
 * 
 */
#define FULL_CONTEXT_FRAME_INIT(task_fn, arg, ret_reg) {        \
    .registers = REGISTER_CACHE_INIT,                           \
    .exc_frame = EXCEPTION_FRAME_INIT(task_fn, arg, ret_reg)    \
}

/* -- Types --------------------------------------------------------------- */

/**
 * @brief Systems register size (32 bits)
 * 
 */
typedef uint32_t sysregister_t;

#pragma pack(push, 1)
/**
 * @brief Register layout for exception frames pushed onto the stack
 * 
 */
typedef struct {
    sysregister_t r0;
    sysregister_t r1;
    sysregister_t r2;
    sysregister_t r3;
    sysregister_t r12;
    sysregister_t lr;
    sysregister_t pc;
    sysregister_t xpsr;
} ExceptionFrame;

/**
 * @brief Additional register layout pushed onto the stack by JOCKTOS
 * 
 */
typedef struct {
    sysregister_t r4;
    sysregister_t r5;
    sysregister_t r6;
    sysregister_t r7;
    sysregister_t r8;
    sysregister_t r9;
    sysregister_t r10;
    sysregister_t r11;
} RegisterCache;

/**
 * @brief Register layout for full context frames pushed onto the stack
 * 
 */
typedef struct {
    RegisterCache registers;
    ExceptionFrame exc_frame;
} FullContextFrame;
#pragma pack(pop)

/* -- Externs (avoid these for library functions) ------------------------- */

/* -- Function Declarations ----------------------------------------------- */
