/**
* \brief This module is to...
*/

/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "os.h"
// Middleware
// Bios
#include "cmsis_gcc.h"
// Standard C

/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Local Globals (not for libraries with application instantiation) ---- */

/* -- Private Function Declarations --------------------------------------- */

/* -- Public Functions----------------------------------------------------- */

/**
* \brief Enables all interrupts on the system
*
* \return none
*/
void vEnableInterrupts()
{
    void __enable_irq(void);    
}

/**
* \brief Disables all interrupts on the system
*
* \return none
*/
void vDisableInterrupts()
{
    void __disable_irq(void);
}