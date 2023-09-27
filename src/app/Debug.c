#include "Debug.h"

#include "GPIO.h"
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

static UART_t * uart0;

/******************************************************************************
Initialization
*******************************************************************************/

void Debug_Init(void) {
    GPIO_Port_t * portA = GPIO_InitPort(A);
    uart0 = UART_Init(portA, UART0);
    Debug_SendMsg(MSG_LIST[START_MSG]);
    return;
}

/******************************************************************************
Serial Output
*******************************************************************************/

void Debug_SendMsg(void * message) {
    UART_WriteStr(uart0, message);
    return;
}

void Debug_SendFromList(msg_t msg) {
    Debug_SendMsg(MSG_LIST[msg]);
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
        Debug_SendFromList(ASSERT_FALSE);
        Assert(false);
    }
    return;
}
