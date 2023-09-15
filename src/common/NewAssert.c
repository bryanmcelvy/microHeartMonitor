/**
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for custom `assert` implementation.
 */

#include "NewAssert.h"

#include <stdbool.h>

static void Assert_Handler(void);

void Assert(bool condition) {
    if(condition == false) {
        Assert_Handler();
    }
}

static void Assert_Handler(void) {
    while(1) {}
}
