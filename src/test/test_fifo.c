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

void FIFO_reportStatus(FIFO_t * fifo_ptr);

int main(void) {
    uint32_t buffer[FIFO_LEN];
    FIFO_t * fifo_ptr = 0;
    uint32_t print_buffer[FIFO_LEN];

    PLL_Init();
    GPIO_PF_LED_Init();
    Timer0A_Init();
    UART0_Init();
    UART0_WriteStr((unsigned char *) "\nTransmission started...\r\n");

    srand(42);

    // Initialize FIFO buffer
    fifo_ptr = FIFO_Init(buffer, FIFO_LEN);
    FIFO_reportStatus(fifo_ptr);

    // Add random numbers to the buffer
    UART0_WriteStr((unsigned char *) "Placing random values into buffer...");
    for(int i = 0; i < FIFO_LEN - 1; i++) {
        FIFO_Put(fifo_ptr, rand());
    }
    UART0_WriteStr((unsigned char *) "done!\n");
    FIFO_reportStatus(fifo_ptr);

    // Show contents of buffer
    UART0_WriteStr((unsigned char *) "Current contents: ");
    UART0_WriteChar('\n');

    FIFO_PeekAll(fifo_ptr, print_buffer);
    for(int i = 0; i < FIFO_LEN - 1; i++) {
        UART0_WriteInt(print_buffer[i]);
        UART0_WriteChar('\n');
    }

    // Remove one at a time
    UART0_WriteStr((unsigned char *) "Removing values...\n");
    FIFO_reportStatus(fifo_ptr);
    for(int i = 0; i < FIFO_LEN - 1; i++) {
        uint32_t tmp_val = FIFO_Get(fifo_ptr);
        UART0_WriteInt(tmp_val);
        UART0_WriteChar('\n');
        FIFO_reportStatus(fifo_ptr);
    }
    UART0_WriteStr((unsigned char *) "Done!\n");

    // Blink
    while(1) {
        GPIO_PF_LED_Toggle(LED_GREEN);
        Timer0A_Wait1ms(500);
    }
}

void FIFO_reportStatus(FIFO_t * fifo_ptr) {
    if(FIFO_isEmpty(fifo_ptr)) {
        UART0_WriteStr((unsigned char *) "FIFO is empty.");
    }
    else if(FIFO_isFull(fifo_ptr)) {
        UART0_WriteStr((unsigned char *) "FIFO is full.");
    }
    else {
        UART0_WriteStr((unsigned char *) "Current size: ");
        UART0_WriteInt(FIFO_getCurrSize(fifo_ptr));
    }
    UART0_WriteChar('\n');
}
