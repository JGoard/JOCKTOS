/**
* \brief This module contains ADC handling functions
*/
/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "os.h"
#include "sys.h"
#include "io.h"
#include "timers.h"
// Middleware
// Bios
// Standard C
#include <stdlib.h>
#include <errno.h>

/* -- Defines ------------------------------------------------------------- */
#define PIN_MAX 15 // 15 pins per GPIO

/* -- Types --------------------------------------------------------------- */

///<TODO:   Maybe in the future we can make this a struct for easier user managerment for inits?
//          There would be one type for each i/o type. It's a better method than just running around 
//          Remembering ports, pins, modes, etc. Maybe there can be an I/O Master list used for initialization
// typedef struct
// {
//     GPIO_TypeDef *port;
//     uint8_t pin;
//     uint8_t mode;
//     uint8_t res;

// } GPIO_DigIn;


/* -- Local Globals (not for libraries with application instantiation) ---- */

/* -- Private Function Declarations --------------------------------------- */


/* -- Public Functions----------------------------------------------------- */


// The temperature sensor is internally connected to the ADC1_IN16 input channel which is 
// used to convert the sensor output voltage into a digital value.
int16_t jock_io_init()
{
    /* Enable clock for GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF */
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    RCC->AHBENR |= RCC_AHBENR_GPIODEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOFEN;


    ///<TODO: Maybe do a mass reset/memset for all pin configurations

}

int16_t jock_io_initDigitalInput(GPIO_TypeDef *port, uint8_t pin, uint8_t res){

    /* General Error Handling for unexpected inputs*/
    if (pin > PIN_MAX || pin < 0 || port == NULL || res > 3)       
    {
        ///<TODO: Set an error for invalid config. Maybe be specific with failures?

        return EACCES;
    }
    /* This will clear the bits in the appropriate positions before setting them statements are not necessary here; 
        But when an option has more than one bit, it is good practice to reset the whole option 
        before setting the bits you want.
    */
    port->MODER = (port->MODER & ~(0x3 << (pin*2))) | (JOCK_IO_MODE_INPUT   << (pin*2));  // Each pin occupies two bits for settings (4 settings total)
    port->PUPDR = (port->PUPDR & ~(0x3 << (pin*2))) | (res                  << (pin*2));  // Each pin occupies two bits for settings (4 settings total)

    return 0;
    }

int16_t jock_io_initDigitalOutput(GPIO_TypeDef *port, uint8_t pin, uint8_t speed, uint8_t type){
    
    int16_t errorState = 0;
    if (pin > PIN_MAX || pin < 0 || port == NULL || speed > 3 || type > 1)       
    {
        ///<TODO: Set an error for invalid config. Maybe be specific with failures?

        return EACCES;
    }
    /* This will clear the bits in the appropriate positions before setting them statements are not necessary here; 
    But when an option has more than one bit, it is good practice to reset the whole option 
    before setting the bits you want.
    */
    port->MODER = (port->MODER  & ~(0x3 << (pin*2))) | (JOCK_IO_MODE_OUTPUT << (pin*2));  // Each pin occupies two bits for settings (4 settings total)
    port->OTYPER= (port->OTYPER & ~(0x1 << (pin*2))) | (type                << (pin));     // OTYPER only has 2 settings total (1 bit)

    return 0;
    }

int16_t  jock_io_getDigitalInput(GPIO_TypeDef *port, uint8_t pin, uint8_t* value){

    if (pin > PIN_MAX || pin < 0 || port == NULL || value == NULL){
        ///<TODO: Set an error for invalid config. Maybe be specific with failures?
        return EACCES;
    }

    /* Error check to confirm that pin is indeed configured as a digital input */
    if ((port->MODER & (0x3 << (pin*2))) != JOCK_IO_MODE_INPUT){
        return ENOMSG;
    }
    else{
    /* Copy entire ports pin values, and then select the pin we want */
    uint32_t digitalInVals = port->IDR;
    *value = (port->IDR >> pin) & 0x1;
    }
    return 0;
}

int16_t  jock_io_setDigitalOutput(GPIO_TypeDef *port, uint8_t pin, uint8_t value){

    if (pin > PIN_MAX || pin < 0 || port == NULL || value == NULL){
        ///<TODO: Set an error for invalid config. Maybe be specific with failures?
        return EACCES;
    }

    /* Error check to confirm that pin is indeed configured as a digital output */
    if ((port->MODER & (0x3 << (pin*2))) != (JOCK_IO_MODE_OUTPUT << (pin*2))){
 
        return ENOMSG;
    }
    else{
    /* Set the ODR bit to the value in which we wish to set from the value passed in */
    port->ODR = (port->ODR & ~(1 << pin)) | ((value & 0x1) << pin);
    }
    return 0;
}

/* The idea is to have the user pass in a reference to the port and pin, that they would like to initialize to the following for basic I/O...
    - Digital Input
    - Analog Input
    - PWM Input

    - Digital Output
    - Analog Output
    - PWM Output 
*/
