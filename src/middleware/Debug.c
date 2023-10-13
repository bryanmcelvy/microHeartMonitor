#include "Debug.h"

#include "GPIO.h"
#include "UART.h"

#include "NewAssert.h"

#include <stdbool.h>

// clang-format off
static const char * const MSG_LIST[] = {
    "Data acquisition module initialized.\r\n", 
    "QRS detection module initialized.\r\n",
    "LCD module initialized.\r\n",
    "Starting QRS detection...\r\n"
};
// clang-format on

static Uart_t debugUart = 0;

/******************************************************************************
Initialization
*******************************************************************************/

void Debug_Init(Uart_t uart) {
    Assert(UART_isInit(uart));

    debugUart = uart;

    Debug_SendMsg((void *) "Starting transmission...\r\n");
    Debug_SendMsg((void *) "Debug module initialized.\r\n");
    return;
}

/******************************************************************************
Serial Output
*******************************************************************************/

void Debug_SendMsg(void * message) {
    UART_WriteStr(debugUart, message);
    return;
}

void Debug_SendFromList(Msg_t msg) {
    Debug_SendMsg((void *) MSG_LIST[msg]);
    return;
}

void Debug_WriteFloat(double value) {
    UART_WriteFloat(debugUart, value, 1);
    UART_WriteStr(debugUart, "\r\n");
    return;
}

/******************************************************************************
Assertions
*******************************************************************************/

void Debug_Assert(bool condition) {
    if(condition == false) {
        Debug_SendMsg((void *) "Assertion failed. Entering infinite loop.\r\n.");
        Assert(false);
    }
    return;
}
