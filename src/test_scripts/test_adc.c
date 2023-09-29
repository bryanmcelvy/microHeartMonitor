/**
 * @file
 * @author  Bryan McElvy
 * @brief   Test script for analog-to-digital conversion (ADC) module.
 */

#include "ADC.h"

#include "PLL.h"
#include "GPIO.h"
#include "Timer.h"
// #include "UART.h"

#include "FIFO.h"

#include "tm4c123gh6pm.h"
#include <stdint.h>

#define LED_PINS           (GPIO_Pin_t)(GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3)

#define SAMPLING_PERIOD_MS (uint32_t) 5
#define NUM_SAMPLES        (uint32_t) 1000

volatile bool buffer_is_full = false;
volatile FIFO_t * fifo_ptr = 0;
volatile uint32_t fifo_buffer[NUM_SAMPLES];

int main(void) {
    bool done_sampling = false;
    // uint32_t samp_buffer[NUM_SAMPLES];

    PLL_Init();

    // UART0_Init();
    // UART0_WriteStr("Starting transmission...\r\n");

    fifo_ptr = FIFO_Init(fifo_buffer, NUM_SAMPLES);

    // Init. LED pins
    GPIO_Port_t * portF = GPIO_InitPort(F);
    GPIO_ConfigDirOutput(portF, LED_PINS);
    GPIO_ConfigDriveStrength(portF, LED_PINS, 8);
    GPIO_EnableDigital(portF, LED_PINS);

    GPIO_Toggle(portF, LED_RED);

    ADC_Init();

    Timer_t DAQ_Timer = Timer_Init(TIMER3);
    Timer_setMode(DAQ_Timer, PERIODIC, UP);
    Timer_enableAdcTrigger(DAQ_Timer);
    Timer_setInterval_ms(DAQ_Timer, SAMPLING_PERIOD_MS);
    Timer_Start(DAQ_Timer);

    while(1) {
        if(done_sampling) {}
        else if(buffer_is_full) {
            // FIFO_Flush(fifo_ptr, samp_buffer);
            done_sampling = true;
            GPIO_Toggle(portF, LED_RED + LED_GREEN);
        }
    }
}

void ADC0_SS3_Handler(void) {
    volatile uint32_t raw_sample;
    // volatile double sample;

    raw_sample = ADC0_SSFIFO3_R & 0xFFF;
    if(buffer_is_full == false) {
        FIFO_Put(fifo_ptr, raw_sample);
        buffer_is_full = FIFO_isFull(fifo_ptr);
    }
    else {
        buffer_is_full = true;
    }

    // sample = ADC_ConvertToVolts(raw_sample);

    // UART0_WriteInt(sample);
    // UART0_WriteFloat(sample, 2);
    // UART0_WriteChar('\n');

    ADC0_ISC_R |= 0x08;               // clear interrupt flag to acknowledge
}
