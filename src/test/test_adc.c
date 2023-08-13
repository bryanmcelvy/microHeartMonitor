/**
 * @file
 * @author  Bryan McElvy
 * @brief   Test script for analog-to-digital conversion (ADC) module.
 */

#include "ADC.h"

// #include "FIFO.h"
// #include "lookup.h"
#include "PLL.h"
#include "Timer.h"
#include "UART.h"

#include "tm4c123gh6pm.h"
#include <stdint.h>

#define SAMPLING_PERIOD_MS 5

int main(void) {
    PLL_Init();
    UART0_Init();
    UART0_WriteStr("Starting transmission...\r\n");

    ADC_Init();
    Timer3A_Init(SAMPLING_PERIOD_MS);

    while(1) {}
}

void ADC0_SS3_Handler(void) {
    uint16_t sample;

    sample = ADC0_SSFIFO3_R & 0xFFF;

    UART0_WriteInt(sample);
    UART0_WriteChar('\n');

    ADC0_ISC_R |= 0x08;                    // clear interrupt flag to acknowledge
}
