#include "Debug.h"

#include "GPIO.h"
#include "UART.h"

#include "NewAssert.h"

#include "tm4c123gh6pm.h"
#include <stdbool.h>
#include <stdint.h>

// clang-format off
static const char * const MSG_LIST[] = {
    "Data acquisition module initialized.\r\n", 
    "QRS detection module initialized.\r\n",
    "LCD module initialized.\r\n", 
};
// clang-format on

static UART_t * uart0;

/******************************************************************************
Initialization
*******************************************************************************/

void Debug_Init(void) {
    GPIO_Port_t * portA = GPIO_InitPort(A);
    uart0 = UART_Init(portA, UART0);
    Debug_SendMsg((void *) "Starting transmission...\r\n");
    Debug_SendMsg((void *) "Debug module initialized.\r\n");
    return;
}

/******************************************************************************
Serial Output
*******************************************************************************/

void Debug_SendMsg(void * message) {
    UART_WriteStr(uart0, message);
    return;
}

void Debug_SendFromList(Msg_t msg) {
    Debug_SendMsg((void *) MSG_LIST[msg]);
    return;
}

void Debug_WriteFloat(double value) {
    UART_WriteFloat(uart0, value, 3);
    UART_WriteStr(uart0, "\r\n");
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
