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

int main(void) {

    PLL_Init();
    Timer0A_Init();
    UART0_Init();

    // Init. LED pins
    GPIO_Port_t * portF = GPIO_InitPort(F);
    GPIO_ConfigDirOutput(portF, (GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3));
    GPIO_ConfigDriveStrength(portF, (GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3), 8);
    GPIO_EnableDigital(portF, (GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3));

    UART0_WriteStr((unsigned char *) "Starting transmission...\r\n");

    counter = 0;
    while(1) {
        in_char = UART0_ReadChar();
        switch(in_char) {
            case((unsigned char) 'r'):
            case((unsigned char) 'R'): GPIO_Toggle(portF, 0x02); break;
            case((unsigned char) 'g'):
            case((unsigned char) 'G'): GPIO_Toggle(portF, 0x08); break;
            case((unsigned char) 'b'):
            case((unsigned char) 'B'): GPIO_Toggle(portF, 0x04); break;
        }
        UART0_WriteChar(in_char);
        counter += 1;
    }
}
