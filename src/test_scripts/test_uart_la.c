/**
 * @file
 * @author  Bryan McElvy
 * @brief   Test script for using a USB logic analyzer to decode UART signals.
 */

#include "PLL.h"
#include "GPIO.h"
#include "Timer.h"
#include "UART.h"

int main(void) {

    PLL_Init();
    Timer0A_Init();

    GPIO_Port_t * portB = GPIO_InitPort(B);
    UART_t * uart1 = UART_Init(portB, UART1);

    UART_WriteStr(uart1, (unsigned char *) "Starting transmission...\r\n");

    while(1) {
        UART_WriteStr(uart1, (unsigned char *) "Hello World!\r\n");
        Timer0A_Wait1ms(1);
    }
}
