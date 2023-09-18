/**
 * @file
 * @author  Bryan McElvy
 * @brief   Header file for custom `assert` implementation.
 */

#ifndef NEW_ASSERT_H
#define NEW_ASSERT_H

#include <stdbool.h>

#undef assert

/**
 * @brief               Custom `assert` implementation that is more lightweight
 *                      than the one from `newlib`.
 *
 * @param[in] condition Conditional to test. Causes an infinite loop if `false`.
 */
void Assert(bool condition);

#endif               // NEW_ASSERT_H
