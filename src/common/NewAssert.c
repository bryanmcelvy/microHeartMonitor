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

    return;
}

static void Assert_Handler(void) {
#ifdef __arm__
    __asm__("BKPT #0");
#else
    while(1) {}
#endif
}
