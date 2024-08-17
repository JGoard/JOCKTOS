/**
 * @file test_utils.h
 *
 * @brief This is a minimal testing utility header that provides a basic
 * framework for writing unit tests in C.
 *
 * @details
 * This header provides the following basic components:
 * 
 * - LOG: Prints a message to the console in the specified color.
 * - TEST_EVAL: Wrapper for test function execution.
 * - TEST_CASE: Wrapper for test case separation (within a test function). This has the following companions:
 * - - CASE_COMPLETE: Indicates the end of a test case.
 * - - CASE_NOT_IMPLEMENTED: Indicates the end of an incomplete test case.
 * 
 * Additionally, the following assertions are provided:
 *
 * - ASSERT_TRUE: Assert that a condition is true.
 * - ASSERT_FALSE: Assert that a condition is false.
 * - ASSERT_EQUAL_PTR: Assert that two pointers are equal.
 * - ASSERT_EQUAL_INT: Assert that two integers are equal.
 * - ASSERT_EQUAL_CHAR: Assert that two characters are equal.
 * - ASSERT_EQUAL_STR: Assert that two strings are equal.
 * - ASSERT_NOT_EQUAL_PTR: Assert that two pointers are not equal.
 * - ASSERT_NOT_EQUAL_INT: Assert that two integers are not equal.
 * - ASSERT_NOT_EQUAL_CHAR: Assert that two characters are not equal.
 * - ASSERT_NOT_EQUAL_STR: Assert that two strings are not equal.
 * 
 * Lastly, the cummulative test status can be retrieved with the functio @ref testGetStatus "testGetStatus()".
 * 
 * @author Nicholas Schneider
 */
#ifndef _TEST_UTILS_H_
#define _TEST_UTILS_H_
/* -- Includes ------------------------------------------------------------ */
// Jocktos
// Middleware
// Bios
// Standard C
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* -- Defines ------------------------------------------------------------- */
#define BLUE    "\x1b[34m"
#define GREEN   "\x1b[32m"
#define RED     "\x1b[31m"
#define RESET   "\x1b[0m"
#define CYAN    "\x1b[36m"
#define MAGENTA "\x1b[35m"
#define YELLOW  "\x1b[33m"

#define LOG(msg, col)   printf(col "%s" RESET, msg);

/**
 * @brief Evaluate a statement and print its name.
 * 
 * @param arg The test function to evaluate.
 */
#define TEST_EVAL(arg)                      \
    printf(MAGENTA "%s():\n" RESET, #arg);  \
    depth++;                                \
    arg();                                  \
    depth--;
    
/**
 * @brief Define a new test case.
 * 
 * @param name The name of the test case.
 */
#define TEST_CASE(name)                         \
    clearCase();                               \
    printIndent();                             \
    printf(BLUE "case: " RESET "%s\n", name);   \
    incDepth();                                \

/**
 * @brief Indicate that the current test case has completed.
 */
#define CASE_COMPLETE                       \
    if(caseHasFailed()) failTest();      \
    else {                                  \
        printIndent();                     \
        printf(GREEN ":: passed\n" RESET);  \
    } decDepth();                          \

/**
 * @brief Print a message indicating that a test case is not yet implemented.
 * 
 */
#define CASE_NOT_IMPLEMENTED                        \
    printIndent();                                 \
    printf(YELLOW "NOT IMPLEMENTED\n" RESET);  \
    decDepth();                                    \

#define ASSERT_BOOL__(cond, cond_str, expression, msg)                              \
    if (cond(expression)) {                                                         \
        failCase();                                                                \
        printIndent();                                                             \
        printf(RED "ASSERT_" cond_str ": [%s] :: %s\n" RESET, #expression, msg);    \
    }

#define ASSERT_TRUE(expression, msg)  ASSERT_BOOL__(!, "TRUE",expression, msg)
#define ASSERT_FALSE(expression, msg) ASSERT_BOOL__( , "FALSE", expression, msg)

#define ASSERT_CHECK__(cond, cond_str, type, a, b, msg)                     \
    if (a cond b) {                                                         \
        failCase();                                                        \
        printIndent();                                                     \
        printf(RED "ASSERT_" cond_str "EQUAL: %s "#cond" %s ["              \
        "%" type " "#cond" %" type "] :: %s\n" RESET, #a, #b, a, b, msg);   \
    }

#define ASSERT_EQUAL_PTR(a, b, msg)           ASSERT_CHECK__(!=, "", "p", a, b, msg)
#define ASSERT_NOT_EQUAL_PTR(a, b, msg)       ASSERT_CHECK__(==, "NOT_", "p", a, b, msg)

#define ASSERT_EQUAL_INT(a, b, msg)           ASSERT_CHECK__(!=, "", "d", a, b, msg)
#define ASSERT_NOT_EQUAL_INT(a, b, msg)       ASSERT_CHECK__(==, "NOT_", "d", a, b, msg)

#define ASSERT_EQUAL_CHAR(a, b, msg)          ASSERT_CHECK__(!=, "", "c", a, b, msg)
#define ASSERT_NOT_EQUAL_CHAR(a, b, msg)      ASSERT_CHECK__(==, "NOT_", "c", a, b, msg)

#define ASSERT_EQUAL_STR(a, b, len, msg)    \
    for (int i = 0; i < (len); i++)  ASSERT_EQUAL_CHAR((a)[i], (b)[i], msg);  
#define ASSERT_NOT_EQUAL_STR(a, b, len, msg)     \
    for (int i = 0; i < (len); i++)  ASSERT_NOT_EQUAL_CHAR((a)[i], (b)[i], msg);

/* -- Types --------------------------------------------------------------- */

/* -- Externs (avoid these for library functions) ------------------------- */

bool test_failed = false; // status of the entire test suite.
bool case_failed = false; // status of the current test
uint16_t depth = 0; //The indentation depth of the current test.

/* -- Function Declarations ----------------------------------------------- */

/**
 * @brief Retrieve the test status.
 * 
 * @return true if any test has failed, false otherwise.
 */
bool testGetStatus() { return test_failed; }

/**
 * @brief Print the current test indent.
 */
static void printIndent() {
    for (int i = 0; i < depth; i++) {
        putchar(' ');
        putchar(' ');
    }
}

/**
 * @brief Mark the current test case as failed.
 */
void failCase() { case_failed = true; }

/**
 * @brief Mark the current test case as passed.
 */
void clearCase() { case_failed = false; }

/**
 * @brief Mark the entire test suite as failed.
 */
void failTest() { test_failed = true; }


/**
 * @brief Increment the test case indentation depth.
 */
void incDepth() { depth++; }

/**
 * @brief Decrement the test case indentation depth.
 */
void decDepth() { depth--; }

/**
 * @brief Retrieve the test case status.
 * 
 * @return true if the current test case has failed, false otherwise.
 */
bool caseHasFailed() { return case_failed; }

#endif // _TEST_UTILS_H_
