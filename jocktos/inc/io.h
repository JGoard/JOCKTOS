/**
* \brief This header is to act as companion header for adc.c
*/
#ifndef _IO_H_
#define _IO_H_
/* -- Includes ------------------------------------------------------------ */
// Jocktos
// Middleware
#include "stm32m4cortex_bsp.h"
// Bios
// Standard C
#include <stdint.h>

/* -- Defines ------------------------------------------------------------- */

/*--- GPIO Input Defs ---------------------------------------*/
#define INPUT_MAP_DEF(...)              \
{                                       \
    .inputId    = UINT16_MAX,           \
    .port       = NULL,                 \
    .pin        = UINT8_MAX,            \
    .params     = NULL,                 \
    __VA_ARGS__                         \
                                        \
                                        \
}                                       \
/*--- GPIO Digital Input Defs ---*/
#define DIGITAL_INPUT_PARAMS_DEF(...)   \
{                                       \
    .mode = JOCK_IO_MODE_INPUT,         \
    .res  = JOCK_IO_NOPUPDR,            \
    __VA_ARGS__                         \
                                        \
}                                       \
/*--- GPIO Altf Input Defs ---*/
#define ALTF_INPUT_PARAMS_DEF(...)      \
{                                       \
    .mode = JOCK_IO_MODE_AF,            \
    .res  = JOCK_IO_PUR,                \
    .altf = JOCK_IO_ALTF0,              \
    .perph = JOCK_IO_PERPH_USART2,     \
    __VA_ARGS__                         \
                                        \
}                                       \
/*--- GPIO ADC Defs ---*/
///<TODO: implement adc

/*--- GPIO Output Defs ---------------------------------------*/
#define OUTPUT_MAP_DEF(...)             \
{                                       \
    .outputId   = UINT16_MAX,           \
    .port       = NULL,                 \
    .pin        = UINT8_MAX,            \
    .params     = NULL,                 \
    __VA_ARGS__                         \
                                        \
                                        \
}                                       \
/*--- GPIO Digital Output Defs ---*/
#define DIGITAL_OUTPUT_PARAMS_DEF(...)  \
{                                       \
    .mode = JOCK_IO_MODE_OUTPUT,        \
    .speed= JOCK_IO_OSPEED_LOW,         \
    .type = JOCK_IO_OTYPE_PP,           \
    __VA_ARGS__                         \
                                        \
}                                       \
/*--- GPIO DAC Defs ---*/
///<TODO: implement dac
/*--- GPIO Altf Output Defs ---*/
#define ALTF_OUTPUT_PARAMS_DEF(...)     \
{                                       \
    .mode = JOCK_IO_MODE_AF,            \
    .speed= JOCK_IO_OSPEED_LOW,         \
    .type = JOCK_IO_OTYPE_PP,           \
    .altf = JOCK_IO_ALTF0,              \
    __VA_ARGS__                         \
                                        \
}                                       \


/*---- GPIO Input Configurations -----------------------------*/
/* Pull-up or pull-down resistor configurations */
#define JOCK_IO_NOPUPDR     0b00    // No pull up or pull down
#define JOCK_IO_PUR         0b01    // Pull up res
#define JOCK_IO_PDR         0b10    // Pull down res

/* GPIO mode configurations */
#define JOCK_IO_MODE_INPUT  0b00    // Input Mode
#define JOCK_IO_MODE_OUTPUT 0b01    // Output Mode
#define JOCK_IO_MODE_AF     0b10    // Alternate Function
#define JOCK_IO_MODE_ADC    0b11    // Analog Mode

/*---- GPIO Output Configurations ----------------------------*/
/* GPIO output speed configurations */
#define JOCK_IO_OSPEED_LOW  0b00    // 2 MHz
#define JOCK_IO_OSPEED_MID  0b01    // 10 MHz
#define JOCK_IO_OSPEED_LOW2 0b10    // 2 MHz
#define JOCK_IO_OSPEED_HIGH 0b11    // 50 MHz

/* GPIO output type configurations */
#define JOCK_IO_OTYPE_PP    0b00    // 2 MHz
#define JOCK_IO_OTYPE_OD    0b01    // 10 MHz

/*---- Alternate Function Types ------------------------------*/
#define JOCK_IO_ALTF0       0b0000
#define JOCK_IO_ALTF1       0b0001
#define JOCK_IO_ALTF2       0b0010
#define JOCK_IO_ALTF3       0b0011
#define JOCK_IO_ALTF4       0b0100
#define JOCK_IO_ALTF5       0b0101
#define JOCK_IO_ALTF6       0b0110
#define JOCK_IO_ALTF7       0b0111
#define JOCK_IO_ALTF8       0b1000
#define JOCK_IO_ALTF9       0b1001
#define JOCK_IO_ALTF10      0b1010
#define JOCK_IO_ALTF11      0b1011
#define JOCK_IO_ALTF12      0b1100
#define JOCK_IO_ALTF13      0b1101
#define JOCK_IO_ALTF14      0b1110
#define JOCK_IO_ALTF15      0b1111

/*---- Peripheral AltF T    ypes -----------*/
#define JOCK_IO_PERPH_NONE     0b0000
#define JOCK_IO_PERPH_USART2   0b0001


/* -- Types --------------------------------------------------------------- */
typedef struct
{
    uint8_t mode;       // Different Input Params from Input, Alternate function, or Analog Mode
    uint8_t speed;      // speed of digital outputs set
    uint8_t type;       // push-pull vs open-drain
    uint8_t altf;       // Alternate function   
    uint8_t perph;      // Peripheral to use
} outputParams;
typedef struct
{
    uint8_t mode;       // Different Input Params from Input, Alternate function, or Analog Mode
    uint8_t res;        // No resistor, Pull up, or pull down resistor settings
    uint8_t altf;       // Alternate function   
    uint8_t perph;      // Peripheral to use
} inputParams;
typedef struct 
{
    uint16_t inputId;
    GPIO_TypeDef *port;
    uint8_t pin;
    inputParams *params;
}InputMap;

typedef struct 
{
    uint16_t outputId;
    GPIO_TypeDef *port;
    uint8_t pin;
    outputParams *params;

}OutputMap;


/* -- Externs (avoid these for library functions) ------------------------- */

/* -- Function Declarations ----------------------------------------------- */
/**
 * \brief Initializes all the I/O components. This should be called right after reset
 *        and before any other jock_io_*() functions are called.
 *
 * \return 0 if successful, otherwise an error code.
 */

int16_t jock_io_init();

/**
 * \brief Initializes a digital input pin.
 *
 * \param port The GPIO port containing the pin to initialize.
 * \param pin  The pin number to initialize.
 * \param res  The pull-up/pull-down resistor setting for the pin.
 *
 * \return 0 if successful, otherwise an error code.
 */
int16_t jock_io_initDigitalInput    (GPIO_TypeDef *port, uint8_t pin, uint8_t res);


/**
 * \brief Gets the current value of a digital input pin.
 *
 * \param port The GPIO port containing the pin to check.
 * \param pin  The pin number to check.
 * \param value A pointer to a uint8_t variable to store the pin value.
 *
 * \return 0 if successful, otherwise an error code.
 */
int16_t jock_io_getDigitalInput     (uint16_t outputId,uint8_t* value);


/**
 * \brief Initializes a digital output pin.
 *
 * \param port The GPIO port containing the pin to initialize.
 * \param pin  The pin number to initialize.
 * \param speed The desired output speed for the pin.
 * \param type The desired output type for the pin.
 *
 * \return 0 if successful, otherwise an error code.
 */
int16_t jock_io_initDigitalOutput   (GPIO_TypeDef *port, uint8_t pin, uint8_t speed, uint8_t type);


/**
 * \brief Sets the value of a digital output pin.
 *
 * \param port The GPIO port containing the pin to set.
 * \param pin  The pin number to set.
 * \param value The desired output value for the pin.
 *
 * \return 0 if successful, otherwise an error code.
 */
int16_t jock_io_setDigitalOutput    (uint16_t outputId, uint8_t value);


/**
 * \brief Gets the index of a digital input in the input list based on its inputId.
 *
 * \param inputId The inputId of the digital input to find.
 *
 * \return The index of the digital input in the input list, or -1 if not found.
 */
int16_t jock_io_getInputIndex   (uint16_t inputId);


/**
 * \brief Gets the index of a digital output in the output list based on its outputId.
 *
 * \param outputId The outputId of the digital output to find.
 *
 * \return The index of the digital output in the output list, or -1 if not found.
 */
int16_t jock_io_getOutputIndex  (uint16_t outputId);






#endif /* _IO_H_ */
