#include "Debug.h"

#include "UART.h"

#include "NewAssert.h"

#include "tm4c123gh6pm.h"
#include <stdbool.h>
#include <stdint.h>

static const char * const MSG_LIST[] = {
    "Starting transmission...\r\nDebug module initialized.\r\n",
    "Data acquisition module initialized.\r\n", "QRS module initialized.\r\n",
    "LCD module initialized.\r\n", "Assert failed. Entering infinite loop.\r\n."
};

/******************************************************************************
Initialization
*******************************************************************************/

void Debug_Init(void) {
    UART0_Init();
    Debug_SendMsg(MSG_LIST[START_MSG]);
    return;
}

/******************************************************************************
Serial Output
*******************************************************************************/

void Debug_SendMsg(void * message) {
    UART0_WriteStr(message);
    return;
}

void Debug_SendFromList(msg_t msg) {
    Debug_SendMsg(MSG_LIST[msg]);
    return;
}

void Debug_WriteFloat(double value) {
    UART0_WriteFloat(value, 3);
    UART0_WriteStr("\r\n");
    return;
}

/******************************************************************************
Assertions
*******************************************************************************/

void Debug_Assert(bool condition) {
    if(condition == false) {
        Debug_SendFromList(ASSERT_FALSE);
        Assert(false);
    }
    return;
}
