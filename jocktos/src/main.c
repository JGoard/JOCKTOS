/**
* \brief This module contains the main function and etc
*/

/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "main.h"
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
int main(void)
{
    int x = 0;
    int y = 100;
    
    while(1)
    {
        x++;
        if (x >= 100) x = 0;
         y--;
        if (y <= 0) y = 100;
    };
}