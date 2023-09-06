/*
void Debug_Assert(bool condition) {
    if(condition == false) {
        Debug_SendFromList(ASSERT_FALSE);
        Debug_AssertHandler();
    }
}

void Debug_AssertHandler(void) {
    while(1) {}
}

*/

#include "NewAssert.h"
#include <stdbool.h>

static void AssertHandler(void);

void Assert(bool condition) {
    if(condition == false) {
        AssertHandler();
    }
}

static void AssertHandler(void) {
    while(1) {}
}
