/**
 * @file
 * @author  Bryan McElvy
 * @brief   Stub function for custom `Assert()` function.
 */

// NOLINTBEGIN

#include "CppUTest/TestHarness.h"

extern "C" {

#include "NewAssert.h"
#include <stdbool.h>

void Assert(bool condition) {
    return;
}
}

// NOLINTEND
