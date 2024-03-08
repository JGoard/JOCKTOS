/**
* \brief This module is to act as companion header for main.c
*/
#ifndef _MAIN_H_
#define _MAIN_H_
/* -- Includes ------------------------------------------------------------ */
// Jocktos
// Middleware
// Bios
// Standard C
#include <stdint.h>

/* -- Defines ------------------------------------------------------------- */
/**
 * @brief Structure of a generic task default
 */

#define T_TASKCONTEXTBLOCK_DEF(...) \
{                                   \
    .tRegisters     = 0,            \
    .u8Priority     = 0,            \
    .u8Name         = 0,            \
    .eState         = eBLOCKED,     \
    .u32Delay       = 0,            \
    __VA_ARGS__                     \
}

/* -- Types --------------------------------------------------------------- */
//=============================================================================================
/** @brief STM32 Cortex-M4 Core Registers */
typedef struct {

    /** @brief R0-R12 are 32-bit general-purpose registers for data operations. */
    uint32_t u32Registers[13];

    /** @brief The Stack Pointer (SP) is register R13.
     *
     * In Thread mode, bit[1] of the CONTROL register indicates the stack pointer to use:
     * - 0: Main Stack Pointer (MSP). This is the reset value.
     * - 1: Process Stack Pointer (PSP).
     * On reset, the processor loads the MSP with the value from address 0x00000000.
     */
    uint32_t * pu32StackPointer;

    /** @brief The Link Register (LR) is register R14.
     *
     * It stores the return information for subroutines, function calls, and exceptions.
     * On reset, the processor loads the LR value 0xFFFFFFFF.
     */
    uint32_t u32LinkRegister;

    /** @brief The Program Counter (PC) is register R15.
     *
     * It contains the current program address. On reset, the processor loads the PC with the
     * value of the reset vector, which is at address 0x00000004. Bit[0] of the value is loaded
     * into the EPSR T-bit at reset and must be 1.
     */
    uint32_t *pu32ProgramCounter;

    /** @brief The Program Status Register (PSR) combines:
     * - Application Program Status Register (APSR)
     * - Interrupt Program Status Register (IPSR)
     * - Execution Program Status Register (EPSR)
     */
    uint32_t u32ProgramStatusRegister;

    /** @brief Exception Markers contain:
     * - Priority Fault Mask Register (PRIMASK)
     * - Fault Mask Register (FAULTMASK)
     * - Base Priority Mask Register (BASEPRI)
     */
    uint32_t u32ExceptionMarkers[3];

    /** @brief The CONTROL register controls the stack used and the privilege level for software
     * execution when the processor is in Thread mode and indicates whether the FPU state is
     * active.
     */
    uint32_t u32ControlRegister;

} T_CoreRegistersDef;

//=============================================================================================
/**
 * @brief Task State Machine Enumeration
 * 
 * @warning super janky image:
 * \code{.unparsed}
 *                                     -------------
 *                      /-->----->---->| SUSPENDED |<-----<-----<--\
 *           gmagma          /               -------------                \
 *                    /                    ^   v                     \
 *                   /          suspend(); |   | resume();            |
 *                  |                      ^   v                      ^
 *                  ^                    ---------   scheduler   -----------
 *                  | suspend();         | READY | >----->-----> | RUNNING |
 *                  ^                    |       | <-----<-----< |         |
 *                  |                    ---------               -----------
 *                   \                       ^                        v
 *                    \                      | event                  |
 *                     \                     ^                       /
 *                      \               -----------                 /
 *                       \<-----<-----<-| BLOCKED |<-----<-----<---/
 *                                      -----------
 * \endcode
 * 
 * @attention Wild inline LaTeX
 * @f[
 * \int_a^b f(x) dx = F(b) - F(a)
 * @f]
 * 
 */                   

typedef enum {
    eRUNNING    = 0,    /**< Currently active task. */
    eREADY      = 1,    /**< In the queue and ready to run. */
    eBLOCKED    = 2,    /**< Awaiting a resource. */
    eSUSPENDED  = 3,    /**< Delayed or intentionally released. */
    eTaskStateAmount
} E_TaskState;

//=============================================================================================
/** 
 * @brief Cortex-M4 Context Control Block
 */
typedef struct {
    T_CoreRegistersDef  tRegisters; ///<    Hold's the current task context
    uint8_t             u8Priority; ///<    The priority of the task
    char*               u8Name;     ///<    Name of the tast
    E_TaskState         eState;     ///<    Defines current task state
    uint32_t            u32Delay;   ///<    Delay in ms on 

} T_TaskContextBlock;


/* -- Externs (avoid these for library functions) ------------------------- */

/* -- Function Declarations ----------------------------------------------- */
int main(void);
__attribute__((naked)) void SysTick_Handler(void);

#endif /* _MAIN_H_ */


