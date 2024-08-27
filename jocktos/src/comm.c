/**
* \brief This module contains communication handling functions
*/
/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "os.h"
#include "sys.h"
#include "io.h"
#include "comm.h"
#include "timers.h"
// Middleware
// Bios
// Standard C
#include <stdlib.h>
#include <errno.h>

/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Local Globals (not for libraries with application instantiation) ---- */

/* -- Private Function Declarations --------------------------------------- */


/* -- Public Functions----------------------------------------------------- */
void jock_comm_uartInit(GPIO_TypeDef *port, uint8_t pin, uint8_t type, uint8_t speed, uint8_t altf){

uint16_t uartdiv = SystemCoreClock / 9600; // Division factor for baud rate;
/*--- UART is only capable on these three sets of pins ---*/
    switch (altf)
    {

    case JOCK_IO_ALTF7:
        RCC -> APB1ENR |= RCC_APB1ENR_USART2EN;  // Enable clock for USART2
        port->MODER     = (port->MODER  & ~(0x3 << (pin*2))) | (JOCK_IO_MODE_AF     << (pin*2));
        port->OTYPER    = (port->OTYPER & ~(0x1 << (pin*2))) | (JOCK_IO_OTYPE_PP    << (pin));
        port->OSPEEDR   = (port->OSPEEDR& ~(0x3 << (pin*2))) | (JOCK_IO_OSPEED_HIGH << (pin*2));  
        port->PUPDR     = (port->PUPDR  & ~(0x3 << (pin*2))) | (JOCK_IO_NOPUPDR     << (pin*2));  // Each pin occupies two bits for settings (4 settings total)
        GPIOA->AFR[0] |= (JOCK_IO_ALTF7 << (pin * 4));  // (11:10:9:8)    = 0:1:1:1   --> AF7 Alternate function for USART2 at Pin PA2
        USART2->BRR = uartdiv;
        USART2->CR1 |= ( USART_CR1_RE | USART_CR1_TE | USART_CR1_UE ); //Enable UART

        break;                                          // (15:14:13:12)  = 0:1:1:1   --> AF7 Alternate function for USART2 at Pin PA3
    case JOCK_IO_ALTF8:
        RCC -> APB2ENR |= RCC_APB1ENR_USART2EN;  // Enable clock for USART1
        GPIOA->AFR[0] |= (JOCK_IO_ALTF8 << (pin * 4));
        USART2->CR1 |= ( USART_CR1_RE | USART_CR1_TE | USART_CR1_UE );

        break;
    default:
        break;
    }

    ///<TODO: Add error checking

}
