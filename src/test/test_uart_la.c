/**
 * @file
 * @author  Bryan McElvy
 * @brief   Test script for using a USB logic analyzer to decode UART signals.
 */

#include "PLL.h"
#include "Timer.h"
#include "UART.h"

int main() {

    PLL_Init();
    Timer0A_Init();
    UART1_Init();

    UART1_WriteStr((unsigned char *) "Starting transmission...\r\n");

    while(1) {
        UART1_WriteStr((unsigned char *) "Hello World!\r\n");
        Timer0A_Wait1ms(1);
    }
}
