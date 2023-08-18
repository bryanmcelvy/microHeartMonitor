/**
 * @file
 * @author  Bryan McElvy
 * @brief   Test script for the data acquisition (DAQ) module.
 */

// Includes
#include "ADC.h"
#include "LCD.h"

#include "GPIO.h"
#include "PLL.h"
#include "Timer.h"

#include "FIFO.h"

#include "arm_math_types.h"
#include <stdbool.h>
#include <stdint.h>

// Macros
#define LCD_TOP_LINE_OFFSET    (uint16_t) 130
#define LCD_TOP_LINE_THICKNESS (uint16_t) 5

#define LCD_SAMPLE_DX          (uint16_t) 1
#define LCD_SAMPLE_DY          (uint16_t) 1
#define LCD_SAMPLE_Y_MIN       (uint16_t) 0
#define LCD_SAMPLE_Y_MAX       (uint16_t) 127

#define DAQ_BUFFER_SIZE        16

// Declarations
volatile FIFO_t * input_fifo_ptr = 0;
volatile uint32_t input_buffer[DAQ_BUFFER_SIZE] = { 0 };
volatile bool sampleReady = false;

// Main Function
int main(void) {
    uint16_t x, y;
    uint16_t sample;

    PLL_Init();

    // Init. LCD
    LCD_Init();
    LCD_toggleInversion();
    LCD_setColor_3bit(LCD_BLACK);
    LCD_drawHLine(LCD_TOP_LINE_OFFSET, LCD_TOP_LINE_THICKNESS);
    LCD_setColor_3bit(LCD_WHITE - LCD_RED);
    LCD_toggleOutput();

    // Init. ADC
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

            // plot sample
            y = (uint16_t) (((float32_t) sample / (float32_t) 4095) * LCD_SAMPLE_Y_MAX);
            // LCD_drawRectangle(x, LCD_SAMPLE_DX, y, LCD_SAMPLE_DY, true);
            LCD_graphSample(x, LCD_SAMPLE_DX, y, LCD_SAMPLE_DY, LCD_SAMPLE_Y_MIN, LCD_SAMPLE_Y_MAX,
                            (LCD_WHITE - LCD_RED));
            x = (x + LCD_SAMPLE_DX) % NUM_ROWS;
        }
    }
}

void ADC0_SS3_Handler(void) {
    FIFO_Put(input_fifo_ptr, (volatile uint32_t)(ADC0_SSFIFO3_R & 0xFFF));
    sampleReady = true;
    ADC0_ISC_R |= 0x08;                    // clear interrupt flag to acknowledge
}
