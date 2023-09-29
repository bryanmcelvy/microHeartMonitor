/**
 * @file
 * @author  Bryan McElvy
 * @brief   Test script for the data acquisition (DAQ) module.
 */

/********************************************************************************/

// Includes
#include "DAQ.h"
#include "Debug.h"
#include "LCD.h"

#include "ADC.h"
#include "PLL.h"

#include "FIFO.h"
#include "ISR.h"
#include "lookup.h"

#include "arm_math_types.h"
#include <stdbool.h>
#include <stdint.h>

// Declarations
#define DAQ_BUFFER_SIZE   128

#define LCD_TOP_LINE      (Y_MAX - 48)
#define LCD_NUM_Y_VALS    128
#define LCD_X_AXIS_OFFSET 32
#define LCD_Y_MIN         (0 + LCD_X_AXIS_OFFSET)
#define LCD_Y_MAX         (LCD_NUM_Y_VALS + LCD_X_AXIS_OFFSET)

volatile FIFO_t * input_fifo_ptr = 0;
volatile uint32_t input_buffer[DAQ_BUFFER_SIZE] = { 0 };
volatile bool sampleReady = false;

void LCD_plotNewSample(uint16_t x, volatile const float32_t sample);

/********************************************************************************/

int main(void) {
    volatile uint16_t raw_sample;
    volatile float32_t sample;
    float32_t prev_sample = 0;
    float32_t intermediate_sample;

    PLL_Init();
    ISR_GlobalDisable();
    Debug_Init();

    // Initialize/configure LCD
    LCD_Init();
    LCD_toggleColorInversion();

    LCD_setColor_3bit(LCD_BLACK_INV);
    LCD_setArea(0, X_MAX, 0, Y_MAX);
    LCD_Draw();

    LCD_setColor_3bit(LCD_WHITE_INV);
    LCD_drawHoriLine(LCD_TOP_LINE, 1);

    LCD_setColor_3bit(LCD_RED_INV);
    LCD_toggleOutput();

    // Initialize DAQ module
    input_fifo_ptr = FIFO_Init(input_buffer, DAQ_BUFFER_SIZE);
    DAQ_Init();

    uint16_t x = 0;
    ISR_GlobalEnable();
    while(1) {
        while(sampleReady == false) {}

        // collect sample
        raw_sample = FIFO_Get(input_fifo_ptr);
        sampleReady = !FIFO_isEmpty(input_fifo_ptr);

        // convert and filter
        sample = ADC_ConvertToVolts(raw_sample);
        sample = DAQ_Filter(sample);

        intermediate_sample = prev_sample + ((sample - prev_sample) / 2);
        LCD_plotNewSample(x, intermediate_sample);
        x = (x + 1) % X_MAX;

        LCD_plotNewSample(x, sample);
        x = (x + 1) % X_MAX;

        prev_sample = sample;
    }
}

/********************************************************************************/

void ADC0_SS3_Handler(void) {
    Debug_Assert(FIFO_isFull(input_fifo_ptr) == false);
    FIFO_Put(input_fifo_ptr, (volatile uint32_t)(ADC0_SSFIFO3_R & 0xFFF));
    sampleReady = true;
    ADC0_ISC_R |= 0x08;               // clear interrupt flag to acknowledge
}

void LCD_plotNewSample(uint16_t x, volatile const float32_t sample) {
    uint16_t y;

    // blank out column
    LCD_setColor_3bit(LCD_BLACK_INV);
    LCD_drawRectangle(x, 1, LCD_Y_MIN, LCD_NUM_Y_VALS, true);

    // plot sample
    LCD_setColor_3bit(LCD_RED_INV);
    y = LCD_X_AXIS_OFFSET +
        ((uint16_t) (((sample + LOOKUP_ADC_MAX) / (LOOKUP_ADC_MAX * 2)) * LCD_NUM_Y_VALS));
    LCD_drawRectangle(x, 1, y, 1, true);
}
