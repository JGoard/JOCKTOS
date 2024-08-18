/**
* \brief This module contains ADC handling functions
*/
/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "os.h"
#include "sys.h"
#include "io.h"
#include "io_list.h"
#include "timers.h"
// Middleware
// Bios
// Standard C
#include <stdlib.h>
#include <errno.h>

/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Local Globals (not for libraries with application instantiation) ---- */
const inputParams   digInputPullupParams= DIGITAL_INPUT_PARAMS_DEF(.res = JOCK_IO_PUR); // Pull-up input
const inputParams   digInputPulldnParams= DIGITAL_INPUT_PARAMS_DEF(.res = JOCK_IO_PDR); // Pull-down input
const outputParams  digOutputParams     = DIGITAL_OUTPUT_PARAMS_DEF();

InputMap inputList[] = 
{
    DIGITAL_INPUT_MAP_DEF(.inputId = 0, .port = GPIOA, .pin = 0,    .params = &digInputPullupParams),
    DIGITAL_INPUT_MAP_DEF(.inputId = 1, .port = GPIOA, .pin = 1,    .params = &digInputPullupParams),
    DIGITAL_INPUT_MAP_DEF(.inputId = 2, .port = GPIOA, .pin = 2,    .params = &digInputPullupParams),
    DIGITAL_INPUT_MAP_DEF(.inputId = 3, .port = GPIOA, .pin = 3,    .params = &digInputPullupParams),
    DIGITAL_INPUT_MAP_DEF(.inputId = 4, .port = GPIOA, .pin = 4,    .params = &digInputPullupParams),
    DIGITAL_INPUT_MAP_DEF(.inputId = 5, .port = GPIOA, .pin = 5,    .params = &digInputPullupParams),
    DIGITAL_INPUT_MAP_DEF(.inputId = DIG_IN_A6, .port = GPIOA, .pin = 6,    .params = &digInputPulldnParams),
    DIGITAL_INPUT_MAP_DEF(.inputId = 7, .port = GPIOA, .pin = 7,    .params = &digInputPullupParams),
    DIGITAL_INPUT_MAP_DEF(.inputId = 8, .port = GPIOA, .pin = 8,    .params = &digInputPullupParams),
    DIGITAL_INPUT_MAP_DEF(.inputId = 9, .port = GPIOA, .pin = 9,    .params = &digInputPullupParams),
    DIGITAL_INPUT_MAP_DEF(.inputId = 10,.port = GPIOA, .pin = 10,   .params = &digInputPullupParams),
    DIGITAL_INPUT_MAP_DEF(.inputId = 11,.port = GPIOA, .pin = 11,   .params = &digInputPullupParams),
    DIGITAL_INPUT_MAP_DEF(.inputId = 12,.port = GPIOA, .pin = 12,   .params = &digInputPullupParams),
    // DIGITAL_INPUT_MAP_DEF(.inputId = 13,.port = GPIOA, .pin = 13,   .params = &digInputPullupParams), /* UNUSABLE: THESE ARE DEBUGGING PINS AND WILL BRICK YOUR MCU */
    // DIGITAL_INPUT_MAP_DEF(.inputId = 14,.port = GPIOA, .pin = 14,   .params = &digInputPullupParams),
    DIGITAL_INPUT_MAP_DEF(.inputId = 13,.port = GPIOA, .pin = 15,   .params = &digInputPullupParams),



};

OutputMap outputList[] = 
{
    DIGITAL_OUTPUT_MAP_DEF(.outputId = 0, .port = GPIOB, .pin = 0,    .params = &digOutputParams),
    DIGITAL_OUTPUT_MAP_DEF(.outputId = 1, .port = GPIOB, .pin = 1,    .params = &digOutputParams),
    DIGITAL_OUTPUT_MAP_DEF(.outputId = 2, .port = GPIOB, .pin = 2,    .params = &digOutputParams),
    DIGITAL_OUTPUT_MAP_DEF(.outputId = DIG_OUT_B3, .port = GPIOB, .pin = 3,    .params = &digOutputParams),
    DIGITAL_OUTPUT_MAP_DEF(.outputId = 4, .port = GPIOB, .pin = 4,    .params = &digOutputParams),
    DIGITAL_OUTPUT_MAP_DEF(.outputId = 5, .port = GPIOB, .pin = 5,    .params = &digOutputParams),
    DIGITAL_OUTPUT_MAP_DEF(.outputId = 6, .port = GPIOB, .pin = 6,    .params = &digOutputParams),
    DIGITAL_OUTPUT_MAP_DEF(.outputId = 7, .port = GPIOB, .pin = 7,    .params = &digOutputParams),
    DIGITAL_OUTPUT_MAP_DEF(.outputId = 8, .port = GPIOB, .pin = 8,    .params = &digOutputParams),
    DIGITAL_OUTPUT_MAP_DEF(.outputId = 9, .port = GPIOB, .pin = 9,    .params = &digOutputParams),
    DIGITAL_OUTPUT_MAP_DEF(.outputId = 10,.port = GPIOB, .pin = 10,   .params = &digOutputParams),
    DIGITAL_OUTPUT_MAP_DEF(.outputId = 11,.port = GPIOB, .pin = 11,   .params = &digOutputParams),
    DIGITAL_OUTPUT_MAP_DEF(.outputId = 12,.port = GPIOB, .pin = 12,   .params = &digOutputParams),
    DIGITAL_OUTPUT_MAP_DEF(.outputId = 13,.port = GPIOB, .pin = 13,   .params = &digOutputParams),
    DIGITAL_OUTPUT_MAP_DEF(.outputId = 14,.port = GPIOB, .pin = 14,   .params = &digOutputParams),
    DIGITAL_OUTPUT_MAP_DEF(.outputId = 15,.port = GPIOB, .pin = 15,   .params = &digOutputParams),



};

uint16_t lengthofInputs     = sizeof(inputList) / sizeof(inputList[0]);
uint16_t lengthofOutputs    = sizeof(outputList) / sizeof(outputList[0]);

/* -- Private Function Declarations --------------------------------------- */


/* -- Public Functions----------------------------------------------------- */

