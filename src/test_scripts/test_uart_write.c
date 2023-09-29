/**
 * @file
 * @author  Bryan McElvy
 * @brief   Test script for writing to serial port via UART0
 */

#include "PLL.h"
#include "GPIO.h"
#include "Led.h"
#include "UART.h"

volatile unsigned char in_char;
uint32_t counter;

int main(void) {

    PLL_Init();

    // Init. UART0
    GPIO_Port_t * portA = GPIO_InitPort(A);
    UART_t * uart0 = UART_Init(portA, UART0);

    // Init. LED pins
    GPIO_Port_t * portF = GPIO_InitPort(F);
    Led_t * redLed = Led_Init(portF, GPIO_PIN1);
    Led_t * greenLed = Led_Init(portF, GPIO_PIN3);
    Led_t * blueLed = Led_Init(portF, GPIO_PIN2);

    UART_WriteStr(uart0, (unsigned char *) "Starting transmission...\r\n");

    counter = 0;
    while(1) {
        in_char = UART_ReadChar(uart0);
        switch(in_char) {
            case((unsigned char) 'r'):
            case((unsigned char) 'R'): Led_Toggle(redLed); break;
            case((unsigned char) 'g'):
            case((unsigned char) 'G'): Led_Toggle(greenLed); break;
            case((unsigned char) 'b'):
            case((unsigned char) 'B'): Led_Toggle(blueLed);
        }
        UART_WriteChar(uart0, in_char);
        counter += 1;
    }
}
