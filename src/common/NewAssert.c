/**
 * @addtogroup common
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for custom `assert` implementation.
 */

#include "NewAssert.h"

#include <stdbool.h>

void Assert(bool condition) {
    if(condition) {
        return;
    }
    else {
#ifdef __arm__
        __asm__("BKPT #0");
#else
        while(1) {}
#endif
    }
}

/** @} */
