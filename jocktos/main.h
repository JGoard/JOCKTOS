
#include <stdint.h>
//=============================================================================================
/** @brief STM32 Cortex-M4 Core Registers */
typedef struct {

    /** @brief R0-R12 are 32-bit general-purpose registers for data operations. */
    uint32_t registers[13];

    /** @brief The Stack Pointer (SP) is register R13.
     *
     * In Thread mode, bit[1] of the CONTROL register indicates the stack pointer to use:
     * - 0: Main Stack Pointer (MSP). This is the reset value.
     * - 1: Process Stack Pointer (PSP).
     * On reset, the processor loads the MSP with the value from address 0x00000000.
     */
    uint32_t *stack_pointer;

    /** @brief The Link Register (LR) is register R14.
     *
     * It stores the return information for subroutines, function calls, and exceptions.
     * On reset, the processor loads the LR value 0xFFFFFFFF.
     */
    uint32_t link_register;

    /** @brief The Program Counter (PC) is register R15.
     *
     * It contains the current program address. On reset, the processor loads the PC with the
     * value of the reset vector, which is at address 0x00000004. Bit[0] of the value is loaded
     * into the EPSR T-bit at reset and must be 1.
     */
    uint32_t *program_counter;

    /** @brief The Program Status Register (PSR) combines:
     * - Application Program Status Register (APSR)
     * - Interrupt Program Status Register (IPSR)
     * - Execution Program Status Register (EPSR)
     */
    uint32_t program_status_register;

    /** @brief Exception Markers contain:
     * - Priority Fault Mask Register (PRIMASK)
     * - Fault Mask Register (FAULTMASK)
     * - Base Priority Mask Register (BASEPRI)
     */
    uint32_t exception_markers[3];

    /** @brief The CONTROL register controls the stack used and the privilege level for software
     * execution when the processor is in Thread mode and indicates whether the FPU state is
     * active.
     */
    uint32_t control_register;

} coreRegistersDef;


//=============================================================================================
/**
 * @brief Task State Machine Enumeration
 * 
 * @warning super janky image:
 * \code{.unparsed}
 *                                     -------------
 *                      /-->----->---->| SUSPENDED |<-----<-----<--\
 *                     /               -------------                \
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
    RUNNING,   /**< Currently active task. */
    READY,     /**< In the queue and ready to run. */
    BLOCKED,   /**< Awaiting a resource. */
    SUSPENDED  /**< Delayed or intentionally released. */
} jocktos_TaskState;

//=============================================================================================
/** 
 * @brief Cortex-M4 Context Control Block
 */
typedef struct {
    coreRegistersDef registers;
    uint8_t priority;
    char* name;
    jocktos_TaskState state;
    uint32_t delay;

} jocktos_TaskContextBlock;

void JocktosMain(void);
