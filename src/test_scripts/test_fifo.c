/**
 * @file
 * @author  Bryan McElvy
 * @brief   Test script for FIFO buffer.
 */

#include "FIFO.h"
#include "PLL.h"
#include "UART.h"
#include "GPIO.h"
#include "Timer.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define FIFO_LEN 10

#define LED_PINS (GPIO_Pin_t)(GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3)

Uart_t uart;

void FIFO_reportStatus(Fifo_t fifo_ptr);

int main(void) {
    uint32_t buffer[FIFO_LEN];
    Fifo_t fifo_ptr = 0;
    uint32_t print_buffer[FIFO_LEN];

    PLL_Init();

    // Init. Timer
    Timer_t timer = Timer_Init(TIMER0);
    Timer_setMode(timer, ONESHOT, UP);

    // Init. LED pins
    GpioPort_t portF = GPIO_InitPort(F);
    GPIO_ConfigDirOutput(portF, LED_PINS);
    GPIO_ConfigDriveStrength(portF, LED_PINS, 8);
    GPIO_EnableDigital(portF, LED_PINS);

    // Init. UART0
    GpioPort_t portA = GPIO_InitPort(A);
    uart = UART_Init(portA, UART0);
    UART_WriteStr(uart, (unsigned char *) "\nTransmission started...\r\n");

    // Init. FIFO buffer
    fifo_ptr = FIFO_Init(buffer, FIFO_LEN);
    FIFO_reportStatus(fifo_ptr);

    // Add random numbers to the buffer
    srand(42);
    UART_WriteStr(uart, (unsigned char *) "Placing random values into buffer...");
    for(int i = 0; i < FIFO_LEN - 1; i++) {
        FIFO_Put(fifo_ptr, rand());
    }
    UART_WriteStr(uart, (unsigned char *) "done!\n");
    FIFO_reportStatus(fifo_ptr);

    // Show contents of buffer
    UART_WriteStr(uart, (unsigned char *) "Current contents: ");
    UART_WriteChar(uart, '\n');

    FIFO_PeekAll(fifo_ptr, print_buffer);
    for(int i = 0; i < FIFO_LEN - 1; i++) {
        UART_WriteInt(uart, print_buffer[i]);
        UART_WriteChar(uart, '\n');
    }

    // Remove one at a time
    UART_WriteStr(uart, (unsigned char *) "Removing values...\n");
    FIFO_reportStatus(fifo_ptr);
    for(int i = 0; i < FIFO_LEN - 1; i++) {
        uint32_t tmp_val = FIFO_Get(fifo_ptr);
        UART_WriteInt(uart, tmp_val);
        UART_WriteChar(uart, '\n');
        FIFO_reportStatus(fifo_ptr);
    }
    UART_WriteStr(uart, (unsigned char *) "Done!\n");

    // Blink
    while(1) {
        GPIO_Toggle(portF, (GpioPin_t) LED_GREEN);
        Timer_Wait1ms(timer, 500);
    }
}

void FIFO_reportStatus(Fifo_t fifo_ptr) {
    if(FIFO_isEmpty(fifo_ptr)) {
        UART_WriteStr(uart, (unsigned char *) "FIFO is empty.");
    }
    else if(FIFO_isFull(fifo_ptr)) {
        UART_WriteStr(uart, (unsigned char *) "FIFO is full.");
    }
    else {
        UART_WriteStr(uart, (unsigned char *) "Current size: ");
        UART_WriteInt(uart, FIFO_getCurrSize(fifo_ptr));
    }
    UART_WriteChar(uart, '\n');
}
