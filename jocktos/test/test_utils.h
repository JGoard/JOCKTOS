/**
 * @brief This is a minimal testing utility header that provides a basic
 * framework for writing unit tests in C.
 * 
 * This header provides the following basic components:
 * 
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
 * - ASSERT_NOT_EQUAL_PTR: Assert that two pointers are not equal.
 * - ASSERT_NOT_EQUAL_INT: Assert that two integers are not equal.
 * 
 * Lastly, the cummulative test status can be retrieved with the function: testGetStatus().
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
#define BLUE "\x1b[34m"
#define GREEN "\x1b[32m"
#define RED "\x1b[31m"
#define RESET "\x1b[0m"
#define CYAN "\x1b[36m"

#define LOG(msg, col)   printf(col "%s" RESET, msg);

/**
 * @brief Evaluate a statement and print its name.
 * 
 * @param arg The test function to evaluate.
 */
#define TEST_EVAL(arg)                      \
    printf("\x1b[35m%s():\033[0m\n", #arg); \
    depth++;                                \
    arg();                                  \
    depth--;
    
/**
 * @brief Define a new test case.
 * 
 * @param name The name of the test case.
 */
#define TEST_CASE(name)                         \
    clear_case();                               \
    print_indent();                             \
    printf("\x1b[34mcase: \033[0m%s\n", name);  \
    inc_depth();                                \

/**
 * @brief Indicate that the current test case has completed.
 */
#define CASE_COMPLETE                           \
    if(get_case_status()) {                     \
        print_indent();                         \
        printf("\x1b[32m:: passed\033[0m\n");   \
    } dec_depth();                              \

/**
 * @brief Print a message indicating that a test case is not yet implemented.
 * 
 */
#define CASE_NOT_IMPLEMENTED                            \
    print_indent();                                     \
    printf("\x1b[33mTEST NOT IMPLEMENTED\033[0m\n");    \
    dec_depth();                                        \

#define __ASSERT_BOOL(cond, cond_str, expression, msg)                                  \
    if (cond(expression)) {                                                             \
        fail_case();                                                                    \
        fail_test();                                                                    \
        print_indent();                                                                 \
        printf("\x1b[31mASSERT_" cond_str ": [%s] :: %s\033[0m\n", #expression, msg);   \
    }

#define ASSERT_TRUE(expression, msg)  __ASSERT_BOOL(!, "TRUE",expression, msg)
#define ASSERT_FALSE(expression, msg) __ASSERT_BOOL( , "FALSE", expression, msg)

#define __ASSERT_CHECK(cond, cond_str, type, a, b, msg)                                                                             \
    if (a cond b) {                                                                                                                 \
        fail_case();                                                                                                                \
        fail_test();                                                                                                                \
        print_indent();                                                                                                             \
        printf("\x1b[31mASSERT_" cond_str "EQUAL: %s "#cond" %s [%" type " "#cond" %" type "] :: %s\033[0m\n", #a, #b, a, b, msg);  \
    }

#define ASSERT_EQUAL_PTR(a, b, msg)           __ASSERT_CHECK(!=, "", "p", a, b, msg)
#define ASSERT_EQUAL_INT(a, b, msg)           __ASSERT_CHECK(!=, "", "d", a, b, msg)
#define ASSERT_NOT_EQUAL_PTR(a, b, msg)       __ASSERT_CHECK(==, "NOT_", "p", a, b, msg)
#define ASSERT_NOT_EQUAL_INT(a, b, msg)       __ASSERT_CHECK(==, "NOT_", "d", a, b, msg)

/* -- Types --------------------------------------------------------------- */

/* -- Externs (avoid these for library functions) ------------------------- */

bool test_failed = false; //status of the entire test suite.
bool case_status = false; // status of the current test
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
static void print_indent() {
    for (int i = 0; i < depth; i++) {
        putchar(' ');
        putchar(' ');
    }
}

/**
 * @brief Mark the current test case as failed.
 */
void fail_case() { case_status = false; }

/**
 * @brief Mark the current test case as passed.
 */
void clear_case() { case_status = true; }

/**
 * @brief Mark the entire test suite as failed.
 */
void fail_test() { test_failed = true; }


/**
 * @brief Increment the test case indentation depth.
 */
void inc_depth() { depth++; }

/**
 * @brief Decrement the test case indentation depth.
 */
void dec_depth() { depth--; }

/**
 * @brief Get the status of the current test case.
 *
 * @return true if the test case has passed, false otherwise.
 */
uint8_t get_case_status() { return case_status; }

#endif // _TEST_UTILS_H_
