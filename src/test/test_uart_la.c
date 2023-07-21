/**
 * @file
 * @author  Bryan McElvy
 * @brief   Test script for using a USB logic analyzer to decode UART signals.
 */

#include "PLL.h"
#include "GPIO.h"
#include "Timer.h"
#include "UART.h"

int main() {

    PLL_Init();
    Timer0A_Init();
    UART0_Init();
    GPIO_PF_LED_Init();

    UART0_WriteStr((unsigned char *) "Starting transmission...\r\n");

    while(1) {
        
    }
}
