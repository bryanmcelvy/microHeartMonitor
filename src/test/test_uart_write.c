/**
 * @file
 * @author  Bryan McElvy
 * @brief   Test script for writing to serial port via UART0
 */

#include "PLL.h"
#include "GPIO.h"
#include "Timer.h"
#include "UART.h"

volatile unsigned char in_char;
uint32_t counter;

int main() {

    PLL_Init();
    Timer0A_Init();
    UART0_Init();
    GPIO_PF_LED_Init();

    UART0_WriteStr((unsigned char *) "Starting transmission...\r\n");

    counter = 0;
    while(1) {
        in_char = UART0_ReadChar();
        switch (in_char) {
            case ((unsigned char) 'r'):
            case ((unsigned char) 'R'):
                GPIO_PF_LED_Toggle(0x02);
                break;
            case ((unsigned char) 'g'):
            case ((unsigned char) 'G'):
                GPIO_PF_LED_Toggle(0x08);
                break;
            case ((unsigned char) 'b'):
            case ((unsigned char) 'B'):
                GPIO_PF_LED_Toggle(0x04);
                break;
        }
        UART0_WriteChar(in_char);
        counter += 1;
    }
}
