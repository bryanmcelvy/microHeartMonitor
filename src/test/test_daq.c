/**
 * @file
 * @author  Bryan McElvy
 * @brief   Test script for the data acquisition (DAQ) module.
 */

// Includes
#include "ADC.h"
#include "DAQ.h"

#include "GPIO.h"
#include "PLL.h"
#include "Timer.h"

#include "FIFO.h"

#include "arm_math_types.h"
#include <stdbool.h>
#include <stdint.h>

#define DAQ_BUFFER_SIZE 16

// Declarations
volatile FIFO_t * input_fifo_ptr = 0;
volatile uint32_t input_buffer[DAQ_BUFFER_SIZE] = { 0 };
volatile bool sampleReady = false;

// Main Function
int main(void) {
    uint16_t x;
    uint16_t sample;

    PLL_Init();

    input_fifo_ptr = FIFO_Init(input_buffer, DAQ_BUFFER_SIZE);
    ADC_Init();
    Timer3A_Init(5);

    GPIO_PF_LED_Init();

    x = 0;
    while(1) {
        if(sampleReady) {
            // collect sample
            ADC_InterruptDisable();
            sample = (uint16_t) FIFO_Get(input_fifo_ptr);
            sampleReady = !FIFO_isEmpty(input_fifo_ptr);
            ADC_InterruptEnable();
        }
    }
}

void ADC0_SS3_Handler(void) {
    FIFO_Put(input_fifo_ptr, (volatile uint32_t)(ADC0_SSFIFO3_R & 0xFFF));
    sampleReady = true;
    ADC0_ISC_R |= 0x08;                    // clear interrupt flag to acknowledge
}
