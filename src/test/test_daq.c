/**
 * @file
 * @author  Bryan McElvy
 * @brief   Test script for the data acquisition (DAQ) module.
 */

/********************************************************************************/

// Includes
#include "Debug.h"
#include "LCD.h"

#include "ADC.h"
#include "GPIO.h"
#include "PLL.h"
#include "Timer.h"

#include "FIFO.h"

#include "arm_math_types.h"
#include <stdbool.h>
#include <stdint.h>

// Declarations
#define DAQ_BUFFER_SIZE 16

volatile FIFO_t * input_fifo_ptr = 0;
volatile uint32_t input_buffer[DAQ_BUFFER_SIZE] = { 0 };
volatile bool sampleReady = false;

/********************************************************************************/

int main(void) {
    uint16_t x, y;
    volatile float32_t sample;

    PLL_Init();
    Debug_Init();

    // Initialize/configure LCD
    LCD_Init();
    LCD_toggleInversion();
    LCD_setColor_3bit(LCD_WHITE - LCD_RED);
    LCD_toggleOutput();

    // Init. ADC
    input_fifo_ptr = FIFO_Init(input_buffer, DAQ_BUFFER_SIZE);
    ADC_Init();
    Timer3A_Init(5);

    GPIO_PF_LED_Init();

    x = 0;
    while(1) {
        while(sampleReady == false) {}

        // collect sample
        ADC_InterruptDisable();
        sample = ADC_ConvertToVolts(FIFO_Get(input_fifo_ptr));
        sampleReady = !FIFO_isEmpty(input_fifo_ptr);
        ADC_InterruptEnable();

        Debug_WriteFloat(sample);

        // plot sample
        y = (uint16_t) ((sample / LOOKUP_ADC_MAX) * (Y_MAX - 1));
        LCD_drawRectangle(x, 1, y, 1, true);
        x = (x + 1) % X_MAX;

        // reset display upon wrapping around to x = 0
        if(x == 0) {
            LCD_setColor_3bit(LCD_WHITE - LCD_BLACK);
            LCD_fill();
            LCD_setColor_3bit(LCD_WHITE - LCD_RED);
        }
    }
}

/********************************************************************************/

void ADC0_SS3_Handler(void) {
    FIFO_Put(input_fifo_ptr, (volatile uint32_t)(ADC0_SSFIFO3_R & 0xFFF));
    sampleReady = true;
    ADC0_ISC_R |= 0x08;               // clear interrupt flag to acknowledge
}
