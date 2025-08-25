/**
* \brief This header is to act as companion header for adc.c
*/
#ifndef _IO_H_
#define _IO_H_
/* -- Includes ------------------------------------------------------------ */
// Jocktos
// Middleware
#include "bsp.h"
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
    .mode = jockIoModeInput,            \
    .res  = jockIoInputResNone,         \
    __VA_ARGS__                         \
                                        \
}                                       \
/*--- GPIO Altf Input Defs ---*/
#define ALTF_INPUT_PARAMS_DEF(...)      \
{                                       \
    .mode = jockIoModeAlternate,        \
    .res  = jockIoInputResPullUp,       \
    .altf = jockIoAltf0,                \
    .perph = jockIoPerphUSART2,         \
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
    .mode = jockIoModeOutput,           \
    .speed= jockIoOutputSpeedLow,       \
    .type = jockIoOutputTypePP,         \
    __VA_ARGS__                         \
                                        \
}                                       \
/*--- GPIO DAC Defs ---*/
///<TODO: implement dac
/*--- GPIO Altf Output Defs ---*/
#define ALTF_OUTPUT_PARAMS_DEF(...)     \
{                                       \
    .mode = jockIoModeAlternate,        \
    .speed= jockIoOutputSpeedLow,       \
    .type = jockIoOutputTypePP,         \
    .altf = jockIoAltf0,                \
    __VA_ARGS__                         \
                                        \
}                                       \

/*---- GPIO Input Configurations -----------------------------*/
typedef enum {
    jockIoInputResNone      = 0b00,    // No pull up or pull down
    jockIoInputResPullUp    = 0b01,    // Pull up res
    jockIoInputResPullDown  = 0b10,    // Pull down res
} jockIoInputRes;

/* GPIO mode configurations */
typedef enum {
    jockIoModeInput         = 0b00,    // Input Mode
    jockIoModeOutput        = 0b01,    // Output Mode
    jockIoModeAlternate     = 0b10,    // Alternate Function
    jockIoModeAnalog        = 0b11,    // Analog Mode
} jockIoMode;

/*---- GPIO Output Configurations ----------------------------*/
typedef enum {
    jockIoOutputSpeedLow    = 0b00,    // 2 MHz
    jockIoOutputSpeedMid    = 0b01,    // 10 MHz
    jockIoOutputSpeedLow2   = 0b10,    // 2 MHz
    jockIoOutputSpeedHigh   = 0b11,    // 50 MHz
} jockIoOutputSpeed;

/* GPIO output type configurations */
typedef enum {
    jockIoOutputTypePP      = 0b00,    // 2 MHz
    jockIoOutputTypeOD      = 0b01,    // 10 MHz
} jockIoOutputType;

/*---- Alternate Function Types ------------------------------*/
typedef enum {
    jockIoAltf0             = 0b0000,
    jockIoAltf1             = 0b0001,
    jockIoAltf2             = 0b0010,
    jockIoAltf3             = 0b0011,
    jockIoAltf4             = 0b0100,
    jockIoAltf5             = 0b0101,
    jockIoAltf6             = 0b0110,
    jockIoAltf7             = 0b0111,
    jockIoAltf8             = 0b1000,
    jockIoAltf9             = 0b1001,
    jockIoAltf10            = 0b1010,
    jockIoAltf11            = 0b1011,
    jockIoAltf12            = 0b1100,
    jockIoAltf13            = 0b1101,
    jockIoAltf14            = 0b1110,
    jockIoAltf15            = 0b1111,
} jockIoAltf;

/*---- Peripheral AltF Types -----------*/  // Will need to heavily expand on these and do run time checking to see if ports match required functions
typedef enum {
    jockIoPerphNone         = 0b0000,
    jockIoPerphUSART2       = 0b0001,
} jockIoPerph;

/* -- Types --------------------------------------------------------------- */
typedef struct
{
    jockIoMode          mode;       // Different Input Params from Input, Alternate function, or Analog Mode
    jockIoOutputSpeed   speed;      // speed of digital outputs set
    jockIoOutputType    type;       // push-pull vs open-drain
    jockIoAltf          altf;       // Alternate function   
    jockIoPerph         perph;      // Peripheral to use
} outputParams;
typedef struct
{
    jockIoMode      mode;       // Different Input Params from Input, Alternate function, or Analog Mode
    jockIoInputRes  res;        // No resistor, Pull up, or pull down resistor settings
    jockIoAltf      altf;       // Alternate function   
    jockIoPerph     perph;      // Peripheral to use
} inputParams;
typedef struct 
{
    uint16_t inputId;
    GPIO_TypeDef *port;
    uint8_t pin;
    const inputParams *params;
}InputMap;

typedef struct 
{
    uint16_t outputId;
    GPIO_TypeDef *port;
    uint8_t pin;
    const outputParams *params;

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
int16_t jock_io_init_digital_input    (GPIO_TypeDef *port, uint8_t pin, jockIoInputRes res);

/**
 * \brief Gets the current value of a digital input pin.
 *
 * This function retrieves the current value of a digital input pin specified by its inputId.
 * It performs error checking to ensure that the inputId is valid and that the pin is configured as a digital input.
 *
 * \param inputId The inputId of the digital input pin to retrieve the value for.
 * \param value A pointer to a uint8_t variable to store the pin value.
 *
 * \return 0 if successful, EACCES if the inputId is invalid, or ENOMSG if the pin is not configured as a digital input.
 */
int16_t jock_io_get_digital_input     (uint16_t inputIndex, uint8_t* value);

/**
 * \brief Initializes a digital output pin on the specified port.
 *
 * This function sets up a digital output pin on the specified port by configuring
 * the MODER, OSPEEDR, and OTYPER registers. It also performs error checking on the input
 * parameters to ensure that they are valid.
 *
 * \param port The port on which to initialize the digital output pin.
 * \param pin The pin number to initialize as a digital output.
 * \param speed The speed configuration for the pin.
 * \param type The type configuration for the pin.
 *
 * \return 0 on success, EACCES on error.
 */
int16_t jock_io_init_digital_output   (GPIO_TypeDef *port, uint8_t pin, jockIoOutputSpeed speed, jockIoOutputType type);

/**
 * \brief Sets the value of a digital output pin specified by its outputIndex.
 *
 * This function sets the value of a digital output pin to the specified value.
 * It performs error checking to ensure that the outputIndex is valid and that the pin is configured as a digital output.
 *
 * \param OutputIndex The outputIndex of the digital output pin to set the value for.
 * \param value The value to set the digital output pin to.
 *
 * \return 0 on success, EACCES if the OutputIndex is invalid, or ENOMSG if the pin is not configured as a digital output.
 */
int16_t jock_io_set_digital_output    (uint16_t outputIndex, uint8_t value);

/**
 * \brief Gets the index of a digital input in the input list based on its inputId.
 *
 * \param inputId The inputId of the digital input to find.
 *
 * \return The index of the digital input in the input list, or -1 if not found.
 */
uint16_t jock_io_get_input_index   (uint16_t inputId);

/**
 * \brief Gets the index of a digital output in the output list based on its outputId.
 *
 * \param outputId The outputId of the digital output to find.
 *
 * \return The index of the digital output in the output list, or -1 if not found.
 */
uint16_t jock_io_get_output_index  (uint16_t outputId);






#endif /* _IO_H_ */
