#pragma once
/* -- Includes ------------------------------------------------------------ */
// Jocktos
#include "tcb.h"
// Middleware
// Bios
// Standard C
#include <stdint.h>

/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/**
 * @brief Type of message, used to decode message code unions
 * 
 */
typedef enum {
    INFO,
    WARNING,
    ERROR,
} JOCKTOSMessageType;

/**
 * @brief Enum of non-critical [info] codes
 * 
 */
typedef enum {
    TASK_CREATED,
    TASK_SUSPENDED,
    TASK_RESUMED,
    TICK_COUNT_WRAP,
} JOCKTOSInfo;

/**
 * @brief Enum of potentially-critical [warning] codes
 * 
 */
typedef enum {
    CONTEXT_SWITCH_PENDING,
    QUEUE_ALMOST_FULL, // TODO: implement
    TASK_DEADLINE_MISSED, // TODO: implement
} JOCKTOSWarning;

/**
 * @brief Enum of critical error codes
 * 
 */
typedef enum {
    FAILED_TO_INSTALL_TASK,
    TASK_TERMINATED,
    STACK_OVERFLOW,
    QUEUE_FULL,
    ALLOCATION_FAILED,
} JOCKTOSError;

/**
 * @brief Union of all possible message codes
 * 
 */
typedef union {
    JOCKTOSInfo info;
    JOCKTOSWarning warning;
    JOCKTOSError error;
} JOCKTOSCode;

/**
 * @brief JOCKTOS message types used for logging
 * 
 */
typedef struct {
    JOCKTOSMessageType type;
    JOCKTOSCode code;
    uint32_t tick_count;
    TaskName task_name;
} JOCKTOSMessage;

/* -- Externs (avoid these for library functions) ------------------------- */

/* -- Function Declarations ----------------------------------------------- */
