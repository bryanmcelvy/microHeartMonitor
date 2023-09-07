/**
 * @file
 * @author  Bryan McElvy
 * @brief   Header file for custom `assert` implementation.
 */

#ifndef __NEW_ASSERT_H__
#define __NEW_ASSERT_H__

#include <stdbool.h>

#undef assert

/**
 * @brief               Custom `assert` implementation that is more lightweight
 *                      than the one from `newlib`.
 *
 * @param[in] condition Conditional to test. Causes an infinite loop if `false`.
 */
void Assert(bool condition);

#endif               // __NEW_ASSERT_H__
