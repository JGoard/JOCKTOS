/**
* \brief This module contains the io list
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
const inputParams   digInputPullupParams= DIGITAL_INPUT_PARAMS_DEF(.res = jockIoInputResPullUp); // Pull-up input
const inputParams   digInputPulldnParams= DIGITAL_INPUT_PARAMS_DEF(.res = jockIoInputResPullDown); // Pull-down input
const outputParams  digOutputParams     = DIGITAL_OUTPUT_PARAMS_DEF();
const outputParams  uart2OutputParams    = ALTF_OUTPUT_PARAMS_DEF(.perph = jockIoPerphUSART2, .altf = jockIoAltf7);
const inputParams   uart2InputParams     = ALTF_INPUT_PARAMS_DEF( .perph = jockIoPerphUSART2, .altf = jockIoAltf7);

InputMap inputList[] = {

    INPUT_MAP_DEF(.inputId = 0, .port = GPIOA, .pin = 0,    .params = &digInputPullupParams),
    INPUT_MAP_DEF(.inputId = 1, .port = GPIOA, .pin = 1,    .params = &digInputPullupParams),
    // INPUT_MAP_DEF(.inputId = 2, .port = GPIOA, .pin = 2,    .params = &digInputPullupParams),    // Used for UART Tx below
    INPUT_MAP_DEF(.inputId = 3, .port = GPIOA, .pin = 3,    .params = &uart2InputParams    ),       // Used for UART Rx
    INPUT_MAP_DEF(.inputId = 4, .port = GPIOA, .pin = 4,    .params = &digInputPullupParams),
    INPUT_MAP_DEF(.inputId = 5, .port = GPIOA, .pin = 5,    .params = &digInputPullupParams),
    INPUT_MAP_DEF(.inputId = DIG_IN_A6, .port = GPIOA, .pin = 6,    .params = &digInputPulldnParams),
    INPUT_MAP_DEF(.inputId = 7, .port = GPIOA, .pin = 7,    .params = &digInputPullupParams),
    INPUT_MAP_DEF(.inputId = 8, .port = GPIOA, .pin = 8,    .params = &digInputPullupParams),
    INPUT_MAP_DEF(.inputId = 9, .port = GPIOA, .pin = 9,    .params = &digInputPullupParams),
    INPUT_MAP_DEF(.inputId = 10,.port = GPIOA, .pin = 10,   .params = &digInputPullupParams),
    INPUT_MAP_DEF(.inputId = 11,.port = GPIOA, .pin = 11,   .params = &digInputPullupParams),
    INPUT_MAP_DEF(.inputId = 12,.port = GPIOA, .pin = 12,   .params = &digInputPullupParams),
    // INPUT_MAP_DEF(.inputId = 13,.port = GPIOA, .pin = 13,   .params = &digInputPullupParams), /* UNUSABLE: THESE ARE DEBUGGING PINS AND WILL BRICK YOUR MCU */
    // INPUT_MAP_DEF(.inputId = 14,.port = GPIOA, .pin = 14,   .params = &digInputPullupParams),
    INPUT_MAP_DEF(.inputId = 13,.port = GPIOA, .pin = 15,   .params = &digInputPullupParams),



};

OutputMap outputList[] = {
    OUTPUT_MAP_DEF(.outputId = 16, .port = GPIOA, .pin = 2,    .params = &uart2OutputParams),         // Used for UART Tx

    OUTPUT_MAP_DEF(.outputId = 0, .port = GPIOB, .pin = 0,    .params = &digOutputParams),
    OUTPUT_MAP_DEF(.outputId = 1, .port = GPIOB, .pin = 1,    .params = &digOutputParams),
    OUTPUT_MAP_DEF(.outputId = 2, .port = GPIOB, .pin = 2,    .params = &digOutputParams),
    OUTPUT_MAP_DEF(.outputId = DIG_OUT_B3, .port = GPIOB, .pin = 3,    .params = &digOutputParams),
    OUTPUT_MAP_DEF(.outputId = 4, .port = GPIOB, .pin = 4,    .params = &digOutputParams),
    OUTPUT_MAP_DEF(.outputId = 5, .port = GPIOB, .pin = 5,    .params = &digOutputParams),
    OUTPUT_MAP_DEF(.outputId = 6, .port = GPIOB, .pin = 6,    .params = &digOutputParams),
    OUTPUT_MAP_DEF(.outputId = 7, .port = GPIOB, .pin = 7,    .params = &digOutputParams),
    OUTPUT_MAP_DEF(.outputId = 8, .port = GPIOB, .pin = 8,    .params = &digOutputParams),
    OUTPUT_MAP_DEF(.outputId = 9, .port = GPIOB, .pin = 9,    .params = &digOutputParams),
    OUTPUT_MAP_DEF(.outputId = 10,.port = GPIOB, .pin = 10,   .params = &digOutputParams),
    OUTPUT_MAP_DEF(.outputId = 11,.port = GPIOB, .pin = 11,   .params = &digOutputParams),
    OUTPUT_MAP_DEF(.outputId = 12,.port = GPIOB, .pin = 12,   .params = &digOutputParams),
    OUTPUT_MAP_DEF(.outputId = 13,.port = GPIOB, .pin = 13,   .params = &digOutputParams),
    OUTPUT_MAP_DEF(.outputId = 14,.port = GPIOB, .pin = 14,   .params = &digOutputParams),
    OUTPUT_MAP_DEF(.outputId = 15,.port = GPIOB, .pin = 15,   .params = &digOutputParams),



};

uint16_t lengthofInputs     = sizeof(inputList) / sizeof(inputList[0]);
uint16_t lengthofOutputs    = sizeof(outputList) / sizeof(outputList[0]);

/* -- Private Function Declarations --------------------------------------- */


/* -- Public Functions----------------------------------------------------- */

