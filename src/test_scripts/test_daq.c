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

#include "arm_math_types.h"
#include <stdbool.h>
#include <stdint.h>

// Declarations
#define DAQ_BUFFER_SIZE   128

#define LCD_TOP_LINE      (LCD_Y_MAX - 48)
#define LCD_NUM_Y_VALS    128
#define LCD_X_AXIS_OFFSET 32
#define LCD_Y_MIN         (0 + LCD_X_AXIS_OFFSET)
#define LCD_Y_MAX         (LCD_NUM_Y_VALS + LCD_X_AXIS_OFFSET)

volatile Fifo_t inputFifo = 0;
volatile uint32_t inputBuffer[DAQ_BUFFER_SIZE] = { 0 };
volatile bool sampleReady = false;

void LCD_plotNewSample(uint16_t x, volatile const float32_t sample);

/********************************************************************************/

int main(void) {
    ISR_GlobalDisable();

    PLL_Init();
    ISR_GlobalDisable();
    Debug_Init();

    // Initialize/configure LCD
    LCD_Init();

    LCD_setColor(LCD_BLACK);
    LCD_Fill();
    LCD_Draw();

    LCD_setColor(LCD_WHITE);
    LCD_drawHoriLine(LCD_TOP_LINE, 1);

    LCD_setColor(LCD_RED);
    LCD_setOutputMode(true);

    // Initialize DAQ module
    inputFifo = FIFO_Init(inputBuffer, DAQ_BUFFER_SIZE);
    DAQ_Init();

    uint16_t x = 0;
    float32_t prev_sample = 0;
    ISR_GlobalEnable();
    while(1) {
        while(sampleReady == false) {}

        // collect sample
        volatile uint16_t raw_sample = FIFO_Get(inputFifo);
        sampleReady = !FIFO_isEmpty(inputFifo);

        // convert and filter
        volatile float32_t sample = DAQ_convertToMilliVolts(raw_sample);
        // Debug_Assert(sample < LOOKUP_ADC_MAX);
        sample = DAQ_NotchFilter(sample);
        // Debug_Assert(sample < LOOKUP_ADC_MAX);

        float32_t intermediate_sample = prev_sample + ((sample - prev_sample) / 2);
        LCD_plotNewSample(x, intermediate_sample);
        x = (x + 1) % LCD_X_MAX;

        LCD_plotNewSample(x, sample);
        x = (x + 1) % LCD_X_MAX;

        prev_sample = sample;
    }
}

/********************************************************************************/

void ADC0_SS3_Handler(void) {
    Debug_Assert(FIFO_isFull(inputFifo) == false);
    uint16_t rawSample = DAQ_readSample();
    FIFO_Put(inputFifo, (volatile uint32_t) rawSample);
    sampleReady = true;

    DAQ_acknowledgeInterrupt();
    return;
}

void LCD_plotNewSample(uint16_t x, volatile const float32_t sample) {
    static float32_t maxVoltage = LOOKUP_DAQ_MAX;

    // blank out column
    LCD_setColor(LCD_BLACK);
    LCD_drawRectangle(x, 1, LCD_Y_MIN, LCD_NUM_Y_VALS, true);

    // plot sample
    maxVoltage = (sample > maxVoltage) ? sample : maxVoltage;
    uint16_t y = LCD_X_AXIS_OFFSET +
                 ((uint16_t) (((sample + maxVoltage) / (maxVoltage * 2)) * LCD_NUM_Y_VALS));
    LCD_setColor(LCD_RED);
    LCD_drawRectangle(x, 1, y, 1, true);

    return;
}
