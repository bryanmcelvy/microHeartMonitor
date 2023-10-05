/**
 * @file
 * @author  Bryan McElvy
 * @brief   Main program file.
 */

/******************************************************************************
Preprocessor Directives
******************************************************************************/

#include "DAQ.h"
#include "Debug.h"
#include "LCD.h"
#include "QRS.h"

#include "FIFO.h"
#include "ISR.h"

#include "PLL.h"

#include "arm_math_types.h"
#include "tm4c123gh6pm.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
Declarations
******************************************************************************/

// Interrupt Service Routines (ISRs)
enum {               // clang-format off
    ADC_VECTOR_NUM = INT_ADC0SS3,
    DAQ_VECTOR_NUM = INT_CAN0,
    LCD_VECTOR_NUM = INT_TIMER1A
};               // clang-format on

static void ADC_Handler(void);
static void DAQ_Handler(void);
static void LCD_Handler(void);

// FIFO Buffers
enum {
    DAQ_BUFFER_CAPACITY = 4,
    DAQ_BUFFER_SIZE = DAQ_BUFFER_CAPACITY + 1,

    QRS_BUFFER_SIZE = QRS_NUM_SAMP + 1,

    LCD_BUFFER_CAPACITY = DAQ_BUFFER_CAPACITY,
    LCD_BUFFER_SIZE = LCD_BUFFER_CAPACITY + 1
};

static volatile FIFO_t * DAQ_Fifo = 0;
static volatile uint32_t DAQ_Buffer[DAQ_BUFFER_SIZE] = { 0 };

static volatile FIFO_t * QRS_Fifo = 0;
static volatile uint32_t QRS_FifoBuffer[QRS_BUFFER_SIZE] = { 0 };
static volatile bool QRS_bufferIsFull = false;

static volatile FIFO_t * LCD_Fifo = 0;
static volatile uint32_t LCD_FifoBuffer[LCD_BUFFER_SIZE] = { 0 };

// Processing Buffer
// NOTE: using just one to avoid running out of RAM
static volatile float32_t QRS_Buffer[QRS_BUFFER_SIZE] = { 0 };

// LCD Info
enum {
    LCD_TOP_LINE = (Y_MAX - 48),
    LCD_NUM_Y_VALS = 128,
    LCD_X_AXIS_OFFSET = 0,
    LCD_Y_MIN = (0 + LCD_X_AXIS_OFFSET),
    LCD_Y_MAX = (LCD_NUM_Y_VALS + LCD_X_AXIS_OFFSET)
};

static void LCD_plotNewSample(uint16_t x, volatile const float32_t sample);

/******************************************************************************
Main
******************************************************************************/

int main(void) {
    PLL_Init();
    Debug_Init();

    // Init. vector table and ISRs
    ISR_GlobalDisable();
    ISR_InitNewTableInRam();

    ISR_addToIntTable(LCD_Handler, LCD_VECTOR_NUM);
    ISR_setPriority(LCD_VECTOR_NUM, 1);
    ISR_Enable(LCD_VECTOR_NUM);

    ISR_addToIntTable(ADC_Handler, ADC_VECTOR_NUM);
    ISR_setPriority(ADC_VECTOR_NUM, 1);
    ISR_Enable(ADC_VECTOR_NUM);

    ISR_addToIntTable(DAQ_Handler, DAQ_VECTOR_NUM);
    ISR_setPriority(DAQ_VECTOR_NUM, 1);
    ISR_Enable(DAQ_VECTOR_NUM);

    // Init. FIFOs
    DAQ_Fifo = FIFO_Init(DAQ_Buffer, DAQ_BUFFER_SIZE);
    QRS_Fifo = FIFO_Init(QRS_FifoBuffer, QRS_BUFFER_SIZE);
    LCD_Fifo = FIFO_Init(LCD_FifoBuffer, LCD_BUFFER_SIZE);

    // Initialize/configure LCD
    LCD_Init();
    LCD_toggleColorInversion();

    LCD_setColor_3bit(LCD_BLACK_INV);
    LCD_setArea(0, X_MAX, 0, Y_MAX);
    LCD_Draw();

    LCD_setColor_3bit(LCD_RED_INV);
    LCD_toggleOutput();

    Debug_SendFromList(DEBUG_LCD_INIT);

    // Init. app. modules
    QRS_Init();
    Debug_SendFromList(DEBUG_QRS_INIT);

    DAQ_Init();
    Debug_SendFromList(DEBUG_DAQ_INIT);

    ISR_GlobalEnable();
    while(1) {
        if(QRS_bufferIsFull == true) {
            // Transfer samples from FIFO
            ISR_Disable(DAQ_VECTOR_NUM);

            FIFO_Flush(QRS_Fifo, (uint32_t *) QRS_Buffer);
            QRS_bufferIsFull = false;

            ISR_Enable(DAQ_VECTOR_NUM);

            // Run QRS detection
            Debug_SendMsg("Starting QRS detection...\r\n");

            QRS_Preprocess(QRS_Buffer, QRS_Buffer);
            float32_t heartRate_bpm = QRS_applyDecisionRules(QRS_Buffer);
            Debug_Assert(isnan(heartRate_bpm) == false);
            Debug_Assert(isinf(heartRate_bpm) == false);

            // Output heart rate (TODO: Write to LCD)
            Debug_WriteFloat(heartRate_bpm);
        }
    }
}

/******************************************************************************
Interrupt Service Routines
******************************************************************************/

static void ADC_Handler(void) {
    Debug_Assert(FIFO_isFull(DAQ_Fifo) == false);

    uint16_t rawSample = DAQ_readSample();
    FIFO_Put(DAQ_Fifo, (volatile uint32_t) rawSample);

    ADC_InterruptAcknowledge();
    ISR_triggerInterrupt(DAQ_VECTOR_NUM);
}

static void DAQ_Handler(void) {
    while(FIFO_isEmpty(DAQ_Fifo) == false) {
        volatile uint16_t rawSample = FIFO_Get(DAQ_Fifo);
        volatile float32_t sample = DAQ_convertToMilliVolts(rawSample);
        sample = DAQ_subtractRunningMean(sample);
        sample = DAQ_NotchFilter(sample);

        Debug_Assert(FIFO_isFull(QRS_Fifo) == false);
        FIFO_Put(QRS_Fifo, *((uint32_t *) (&sample)));
        if(FIFO_isFull(QRS_Fifo)) {
            QRS_bufferIsFull = true;
        }

        sample = DAQ_BandpassFilter(sample);
        Debug_Assert(FIFO_isFull(LCD_Fifo) == false);
        FIFO_Put(LCD_Fifo, *((uint32_t *) (&sample)));
    }

    if(QRS_bufferIsFull == false) {
        ISR_triggerInterrupt(LCD_VECTOR_NUM);
    }
}

static void LCD_Handler(void) {
    static float32_t sampPrev = 0;
    static uint16_t x = 0;

    Debug_Assert(FIFO_isEmpty(LCD_Fifo) == false);

    while(FIFO_isEmpty(LCD_Fifo) == false) {
        float32_t sample;
        *((uint32_t *) &sample) = FIFO_Get(LCD_Fifo);

        float32_t intermediate_sample = sampPrev + ((sample - sampPrev) / 2);

        LCD_plotNewSample(x, intermediate_sample);
        x = (x + 1) % X_MAX;

        LCD_plotNewSample(x, sample);
        x = (x + 1) % X_MAX;

        sampPrev = sample;
    }
}

static void LCD_plotNewSample(uint16_t x, volatile const float32_t sample) {
    static const float32_t maxVoltage = LOOKUP_DAQ_MAX * 2;

    // blank out column
    LCD_setColor_3bit(LCD_BLACK_INV);
    LCD_drawRectangle(x, 1, LCD_Y_MIN, LCD_NUM_Y_VALS, true);

    // plot sample
    uint16_t y =
        LCD_Y_MIN + ((uint16_t) (((sample + maxVoltage) / (maxVoltage * 2)) * LCD_NUM_Y_VALS));
    LCD_setColor_3bit(LCD_RED_INV);
    LCD_drawRectangle(x, 1, y, 1, true);

    return;
}
