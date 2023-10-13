#include "Debug.h"

#include "GPIO.h"
#include "UART.h"

#include "NewAssert.h"

#include <stdbool.h>

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
    switch(msg) {
        case DEBUG_DAQ_INIT:
            Debug_SendMsg("Data acquisition module initialized.\r\n");
            break;
        case DEBUG_QRS_INIT:
            Debug_SendMsg("QRS detection module initialized.\r\n");
            break;
        case DEBUG_LCD_INIT:
            Debug_SendMsg("LCD module initialized.\r\n");
            break;
        case DEBUG_QRS_START:
            Debug_SendMsg("Starting QRS detection...\r\n");
            break;
        default:
            Assert(false);
    }
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
