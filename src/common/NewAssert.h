/**
 * @addtogroup common
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Header file for custom `assert` implementation.
 */

#ifndef NEW_ASSERT_H
#define NEW_ASSERT_H

#include <stdbool.h>

#ifdef assert
#error "assert should not already be defined."
#endif

/**
 * @brief               Custom `assert` implementation that is more lightweight
 *                      than the one from `newlib`.
 *
 * @param[in] condition Conditional to test.
 *
 * @post                If `condition == true`, the function simply returns.
 * @post                If `condition == false`, a breakpoint is initiated.
 */
void assert(bool condition);

#endif                  // NEW_ASSERT_H

/** @} */               // common
