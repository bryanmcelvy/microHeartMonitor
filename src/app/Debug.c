#include "Debug.h"

#include "UART.h"

#include "NewAssert.h"

#include "arm_math_types.h"
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
}

/******************************************************************************
Serial Output
*******************************************************************************/

void Debug_SendMsg(void * message) {
    UART0_IRQ_AddStr(message);
    UART0_IRQ_Start();
}

void Debug_SendFromList(uint8_t msg_idx) {
    Debug_SendMsg(MSG_LIST[msg_idx]);
}

void Debug_WriteFloat(float64_t value) {
    UART0_WriteFloat(value, 3);
    UART0_WriteStr("\r\n");
}

/******************************************************************************
Assertions
*******************************************************************************/

void Debug_Assert(bool condition) {
    if(condition == false) {
        Debug_SendFromList(ASSERT_FALSE);
        Assert(false);
    }
}
