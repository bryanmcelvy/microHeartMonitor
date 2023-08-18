#include "Debug.h"

#include "UART.h"

#include "tm4c123gh6pm.h"
#include <stdint.h>

char * const MSG_LIST[] = { "Starting transmission...\r\nDebug module initialized.\r\n",
                            "Data acquisition module initialized.\r\n",
                            "QRS module initialized.\r\n", "LCD module initialized.\r\n" };

void Debug_Init(void) {
    UART0_Init();
    Debug_SendMsg(MSG_LIST[START_MSG]);
}

void Debug_SendMsg(void * message) {
    UART0_IRQ_AddStr(message);
    UART0_IRQ_Start();
}

void Debug_SendFromList(uint8_t msg_idx) {
    Debug_SendMsg(MSG_LIST[msg_idx]);
}
