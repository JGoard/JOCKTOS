/**
* \brief This module contains ADC handling functions
*/
/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "os.h"
#include "sys.h"
#include "io.h"
#include "comm.h"
#include "io_list.h"
#include "timers.h"
// Middleware
// Bios
// Standard C
#include <stdlib.h>
#include <errno.h>

/* -- Defines ------------------------------------------------------------- */
#define PIN_MAX 15 // 15 pins per GPIO
#define PORT_ID(port) ((port == GPIOA) ? 0 : (port == GPIOB) ? 1 : (port == GPIOC) ? 2 : (port == GPIOD) ? 3 : (port == GPIOE) ? 4 : (port == GPIOF) ? 5 : -1)

#define CHECK_BIT(x, pos)                           ((x & (1 << pos)) != 0)
#define SHIFTED_BITMASK(bitmask, pin, multiple)     (bitmask << (pin*multiple))
#define SET_BIT(byte, nbit)                         ((byte) |=(1U << (nbit)))


/* -- Types --------------------------------------------------------------- */

/* -- Local Globals (not for libraries with application instantiation) ---- */

/* -- Private Function Declarations --------------------------------------- */
void _jock_io_initGPIO();
/* -- Public Functions----------------------------------------------------- */

int16_t jock_io_init()
{
    int16_t error = 0;
    /* Enable clock for GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF */
    _jock_io_initGPIO();

    /* Initialize Inputs as needed */
    for (uint16_t i = 0; i < lengthofInputs; i++){

        /* Initialize Inputs as needed*/
        switch (inputList[i].params->mode){
            case JOCK_IO_MODE_INPUT:
                    error |= jock_io_initDigitalInput(  inputList[i].port, 
                                                        inputList[i].pin, 
                                                        inputList[i].params->res);
                break;
            case JOCK_IO_MODE_ADC:
                    // jock_adc_initAnalogInput(  inputList[i].port);

                break;
            case JOCK_IO_MODE_AF:
                ///<TODO: insert function for this as there are many alternate functions
                switch (inputList[i].params->perph)
                {
                case JOCK_IO_PERPH_USART2:
                    jock_comm_uartInit( outputList[i].port, ///<TODO: Add error checking if possible
                                        inputList[i].pin, 
                                        0,
                                        0,
                                        inputList[i].params->altf);
                    break;
                
                default:
                    break;
                }
                break;
                break;
            
            default:
                ///<TODO: Set an error for invalid config. Maybe be specific with failures?
                break;
        }
    }
    /* Initialize Outputs as needed */
    for (uint16_t i = 0; i < lengthofOutputs; i++){
        /* Enable clock for each port if they're instantiated at least once */
        switch (outputList[i].params->mode){
        case JOCK_IO_MODE_OUTPUT:
            error |=jock_io_initDigitalOutput(  outputList[i].port, 
                                                outputList[i].pin, 
                                                outputList[i].params->speed, 
                                                outputList[i].params->type);
            break;
        case JOCK_IO_MODE_ADC:
            break;
        case JOCK_IO_MODE_AF:
            ///<TODO: insert function for this
            switch (outputList[i].params->perph){
            case JOCK_IO_PERPH_USART2: 
                jock_comm_uartInit( outputList[i].port,     ///<TODO: Add error checking if possible
                                    outputList[i].pin, 
                                    outputList[i].params->type,
                                    outputList[i].params->speed,
                                    outputList[i].params->altf);
                break;
            
            default:
                break;
            }     
            break;
        
        default:
            ///<TODO: Set an error for invalid config. Maybe be specific with failures?
            break;
        }
    }

    return 0;
}

int16_t jock_io_initDigitalInput(GPIO_TypeDef *port, uint8_t pin, uint8_t res){

    /* General Error Handling for unexpected inputs*/
    if (pin > PIN_MAX || port == NULL || res > 3)       
    {
        ///<TODO: Set an error for invalid config. Maybe be specific with failures?

        return EACCES;
    }
    /* This will clear the bits in the appropriate positions before setting them statements are not necessary here; 
        But when an option has more than one bit, it is good practice to reset the whole option 
        before setting the bits you want.
    */
    port->MODER     = (port->MODER  &~ SHIFTED_BITMASK(0x3, pin, 2)) | SHIFTED_BITMASK(JOCK_IO_MODE_INPUT,  pin, 2);// Each pin occupies two bits for settings (4 settings total)
    port->PUPDR     = (port->PUPDR  &~ SHIFTED_BITMASK(0x3, pin, 2)) | SHIFTED_BITMASK(res,                 pin, 2);
    return 0;
}

int16_t jock_io_getDigitalInput (uint16_t inputId, uint8_t* value){

    /* General Error Handling for unexpected inputs*/
    if (inputId > MAX_INPUTS || value == NULL){
        ///<TODO: Set an error for invalid config. Maybe be specific with failures?
        return EACCES;
    }

    /* Error check to confirm that pin is indeed configured as a digital input */
    else if ((inputList[inputId].port->MODER & SHIFTED_BITMASK(0x3, inputList[inputId].pin, 2)) != JOCK_IO_MODE_INPUT){
        return ENOMSG;
    }
    else{
    /* Copy entire ports pin values, and then select the pin we want */
    *value = CHECK_BIT(inputList[inputId].port->IDR, inputList[inputId].pin);
    }
    return 0;
}

int16_t jock_io_initDigitalOutput(GPIO_TypeDef *port, uint8_t pin, uint8_t speed, uint8_t type){
    
    /* General Error Handling for unexpected inputs*/
    if (pin > PIN_MAX || port == NULL || speed > 3 || type > 1){
        ///<TODO: Set an error for invalid config. Maybe be specific with failures?
        return EACCES;
    }
    /* This will clear the bits in the appropriate positions before setting them statements are not necessary here; 
    But when an option has more than one bit, it is good practice to reset the whole option 
    before setting the bits you want.
    */
    port->MODER     = (port->MODER  &~ SHIFTED_BITMASK(0x3, pin, 2)) | SHIFTED_BITMASK(JOCK_IO_MODE_OUTPUT,  pin, 2);// Each pin occupies two bits for settings (4 settings total)
    port->OSPEEDR   = (port->OSPEEDR&~ SHIFTED_BITMASK(0x3, pin, 2)) | SHIFTED_BITMASK(speed,                pin, 2);  
    port->OTYPER    = (port->OTYPER &~ SHIFTED_BITMASK(0x1, pin, 2)) | SHIFTED_BITMASK(type,                 pin, 1);  // OTYPER only has 2 settings total (1 bit)

    return 0;
    }

int16_t  jock_io_setDigitalOutput(uint16_t outputId, uint8_t value){

    if (outputId > MAX_OUTPUTS || value > 1){
        ///<TODO: Set an error for invalid config. Maybe be specific with failures?
        return EACCES;
    }

    /* Error check to confirm that pin is indeed configured as a digital output */
    if ((outputList[outputId].port->MODER & SHIFTED_BITMASK(0x3, outputList[outputId].pin, 2)) == JOCK_IO_MODE_OUTPUT){ 
        return ENOMSG;
    }
    else{
    /* Set the ODR bit to the value in which we wish to set from the value passed in */
    SET_BIT(outputList[outputId].port->ODR, outputList[outputId].pin);

    }
    return 0;
}

uint16_t jock_io_getInputIndex(uint16_t inputId){
    for (uint16_t i = 0; i < lengthofInputs; i++)
    {
        if(inputList[i].inputId == inputId){
            return i;   // Return the index if found
        }
    }
    return -1;  // If not found, return -1
}

uint16_t jock_io_getOutputIndex(uint16_t outputId){
    for (uint16_t i = 0; i < lengthofOutputs; i++)
    {
        if(outputList[i].outputId == outputId){
            return i;   // Return the index if found
        }
    }
    return -1;  // If not found, return -1
}

/* The idea is to have the user pass in a reference to the port and pin, that they would like to initialize to the following for basic I/O...
    - Digital Input
    - Analog Input
    - PWM Input

    - Digital Output
    - Analog Output
    - PWM Output 
*/

/*----- Private Functions -----*/

/**
 * \brief Initializes the GPIO ports by enabling their clocks.
 * 
 * This function iterates over the input and output lists, and for each port, 
 * it checks if the clock is already enabled. If not, it enables the clock for 
 * the corresponding GPIO port.
 * 
 * \param None
 * \return None
 */
void _jock_io_initGPIO(){
    /* Enable clock for each port if they're instantiated at least once */
    uint8_t enabledPorts = 0;
    for (uint16_t j = 0; j < lengthofInputs; j++){
        switch (PORT_ID(inputList[j].port)){
            case 0:
                if (!(enabledPorts & (1 << 0))){
                    RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // Enable clock for GPIO Port A
                    enabledPorts |= (1 << 0);
                }
                break;
            case 1:
                if (!(enabledPorts & (1 << 1))){
                    RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // Enable clock for GPIO Port B
                    enabledPorts |= (1 << 1);
                }                break;
            case 2:
                if (!(enabledPorts & (1 << 2))){
                    RCC->AHBENR |= RCC_AHBENR_GPIOCEN; // Enable clock for GPIO Port C
                    enabledPorts |= (1 << 2);
                }                break;            
            case 3:
                if (!(enabledPorts & (1 << 3))){
                    RCC->AHBENR |= RCC_AHBENR_GPIODEN; // Enable clock for GPIO Port D
                    enabledPorts |= (1 << 3);
                }                break;
            case 4:
                if (!(enabledPorts & (1 << 4))){
                    RCC->AHBENR |= RCC_AHBENR_GPIOEEN; // Enable clock for GPIO Port E
                    enabledPorts |= (1 << 4);
                }                break;
            case 5:
                if (!(enabledPorts & (1 << 5))){
                    RCC->AHBENR |= RCC_AHBENR_GPIOFEN; // Enable clock for GPIO Port F
                    enabledPorts |= (1 << 5);
                }                break;
            default:
                break;
        }
        /* If all ports are enabled, break out of the loop */
        if(enabledPorts == 0b00111111){
            return;
        }
    }

    for (uint16_t j = 0; j < lengthofOutputs; j++){
        switch (PORT_ID(outputList[j].port)){
            case 0:
                if (!(enabledPorts & (1 << 0))){
                    RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // Enable clock for GPIO Port A
                    enabledPorts |= (1 << 0);
                }
                break;
            case 1:
                if (!(enabledPorts & (1 << 1))){
                    RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // Enable clock for GPIO Port B
                    enabledPorts |= (1 << 1);
                }                break;
            case 2:
                if (!(enabledPorts & (1 << 2))){
                    RCC->AHBENR |= RCC_AHBENR_GPIOCEN; // Enable clock for GPIO Port C
                    enabledPorts |= (1 << 2);
                }                break;            
            case 3:
                if (!(enabledPorts & (1 << 3))){
                    RCC->AHBENR |= RCC_AHBENR_GPIODEN; // Enable clock for GPIO Port D
                    enabledPorts |= (1 << 3);
                }                break;
            case 4:
                if (!(enabledPorts & (1 << 4))){
                    RCC->AHBENR |= RCC_AHBENR_GPIOEEN; // Enable clock for GPIO Port E
                    enabledPorts |= (1 << 4);
                }                break;
            case 5:
                if (!(enabledPorts & (1 << 5))){
                    RCC->AHBENR |= RCC_AHBENR_GPIOFEN; // Enable clock for GPIO Port F
                    enabledPorts |= (1 << 5);
                }                break;
            default:
                break;
        }
        /* If all ports are enabled, break out of the loop */
        if(enabledPorts == 0b00111111){
            return;
        }
    }
return;
}

