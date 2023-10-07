/**
 * @addtogroup main
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Main program file.
 */

/******************************************************************************
SECTIONS
        Preprocessor Directives
        Declarations
        Main
        Interrupt Service Routines (ISRs)
        Static Function Definition
******************************************************************************/

/******************************************************************************
Preprocessor Directives
******************************************************************************/

#include "DAQ.h"
#include "Debug.h"
#include "LCD.h"
#include "lookup.h"
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
enum {
    DAQ_VECTOR_NUM = INT_ADC0SS3,
    PROC_VECTOR_NUM = INT_CAN0,
    LCD_VECTOR_NUM = INT_TIMER1A
};

static void DAQ_Handler(void);
static void Processing_Handler(void);
static void LCD_Handler(void);

// FIFO Buffers
enum {
    DAQ_BUFFER_CAPACITY = 3,
    DAQ_BUFFER_SIZE = DAQ_BUFFER_CAPACITY + 1,

    QRS_BUFFER_SIZE = QRS_NUM_SAMP + 1,

    LCD_BUFFER_CAPACITY = DAQ_BUFFER_CAPACITY,
    LCD_BUFFER_SIZE = LCD_BUFFER_CAPACITY + 1
};

static volatile Fifo_t DAQ_Fifo = 0;
static volatile uint32_t DAQ_Buffer[DAQ_BUFFER_SIZE] = { 0 };

static volatile Fifo_t QRS_Fifo = 0;
static volatile uint32_t QRS_FifoBuffer[QRS_BUFFER_SIZE] = { 0 };
static volatile bool QRS_bufferIsFull = false;

static volatile Fifo_t LCD_Fifo = 0;
static volatile uint32_t LCD_FifoBuffer[LCD_BUFFER_SIZE] = { 0 };

// Processing Buffer
// NOTE: using just one to avoid running out of RAM
static volatile float32_t QRS_processingBuffer[QRS_BUFFER_SIZE] = { 0 };

// LCD Info
enum {
    LCD_TOP_LINE = (LCD_Y_MAX - 48),

    LCD_WAVE_NUM_Y = LCD_TOP_LINE,

    LCD_WAVE_X_OFFSET = 0,
    LCD_WAVE_Y_MIN = (0 + LCD_WAVE_X_OFFSET),

    LCD_WAVE_Y_MAX = (LCD_WAVE_NUM_Y + LCD_WAVE_X_OFFSET)
};

/******************************************************************************
Main
******************************************************************************/

/**
 * @brief           Main function for the project.
 * @details         Moves the interrupt vector table to RAM; configures and
 *                  enables the ISRs; initializes all modules and static variables;
 *                  and performs QRS detection once the buffer has been filled.
 *
 * @callgraph
 */
int main(void) {
    PLL_Init();
    Debug_Init();

    // Init. vector table and ISRs
    ISR_GlobalDisable();
    ISR_InitNewTableInRam();

    ISR_addToIntTable(DAQ_Handler, DAQ_VECTOR_NUM);
    ISR_setPriority(DAQ_VECTOR_NUM, 1);
    ISR_Enable(DAQ_VECTOR_NUM);

    ISR_addToIntTable(Processing_Handler, PROC_VECTOR_NUM);
    ISR_setPriority(PROC_VECTOR_NUM, 1);
    ISR_Enable(PROC_VECTOR_NUM);

    ISR_addToIntTable(LCD_Handler, LCD_VECTOR_NUM);
    ISR_setPriority(LCD_VECTOR_NUM, 1);
    ISR_Enable(LCD_VECTOR_NUM);

    // Init. FIFOs
    DAQ_Fifo = FIFO_Init(DAQ_Buffer, DAQ_BUFFER_SIZE);
    QRS_Fifo = FIFO_Init(QRS_FifoBuffer, QRS_BUFFER_SIZE);
    LCD_Fifo = FIFO_Init(LCD_FifoBuffer, LCD_BUFFER_SIZE);

    // Init./config. LCD
    LCD_Init();
    LCD_setOutputMode(false);

    LCD_setColor(LCD_BLACK_INV);
    LCD_Fill();

    LCD_setColor(LCD_WHITE_INV);
    LCD_drawHoriLine(LCD_TOP_LINE, 1);

    LCD_setColor(LCD_RED_INV);
    LCD_setOutputMode(true);

    Debug_SendFromList(DEBUG_LCD_INIT);

    // Init. app. modules
    QRS_Init();
    Debug_SendFromList(DEBUG_QRS_INIT);

    DAQ_Init();
    Debug_SendFromList(DEBUG_DAQ_INIT);

    ISR_GlobalEnable();
    while(1) {
        if(QRS_bufferIsFull) {               // flag set by Processing_Handler()
            // Transfer samples from FIFO
            ISR_Disable(PROC_VECTOR_NUM);

            FIFO_Flush(QRS_Fifo, (uint32_t *) QRS_processingBuffer);
            QRS_bufferIsFull = false;

            ISR_Enable(PROC_VECTOR_NUM);

            // Run QRS detection
            Debug_SendMsg("Starting QRS detection...\r\n");

            QRS_Preprocess(QRS_processingBuffer, QRS_processingBuffer);
            float32_t heartRate_bpm = QRS_applyDecisionRules(QRS_processingBuffer);
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

/**
 *
 * @brief   Reads ADC output, converts to raw voltage sample, and sends to next FIFO.
 * @details This ISR has a priority level of 1, is triggered when the ADC
 *          has finished capturing a sample, and also triggers the intermediate processing
 *          handler.
 *
 * @pre     Initialize the DAQ module.
 * @post    The converted sample is placed in the DAQ FIFO, and the DAQ ISR is triggered.
 *
 * @see     DAQ_Init(), Processing_Handler()
 *
 * @callgraph
 */
static void DAQ_Handler(void) {
    // read sample and convert to `float32_t`
    uint16_t rawSample = DAQ_readSample();
    volatile float32_t sample = DAQ_convertToMilliVolts(rawSample);

    // send to intermediate processing handler
    Debug_Assert(FIFO_isFull(DAQ_Fifo) == false);
    FIFO_Put(DAQ_Fifo, *((uint32_t *) &sample));
    ISR_triggerInterrupt(PROC_VECTOR_NUM);

    DAQ_acknowledgeInterrupt();
}

/**
 * @brief   Removes noise from the signal and sends it to the QRS and LCD FIFO buffers.
 * @details This ISR has a priority level of 1, is triggered by the DAQ ISR, and triggers
 *          the LCD Handler. It also notifies the superloop in @ref main() that the QRS
 *          buffer is full.
 *
 * @post    The converted sample is placed in the DAQ FIFO, and the DAQ ISR is triggered.
 *
 * @see     DAQ_Handler(), main(), LCD_Handler()
 *
 * @callgraph
 */
static void Processing_Handler(void) {
    static float32_t sum = 0;
    static uint32_t N = 0;

    // NOTE: this `while` is only here in case a sample arrives while the QRS FIFO is being emptied
    while(FIFO_isEmpty(DAQ_Fifo) == false) {
        volatile float32_t sample;
        *((uint32_t *) &sample) = FIFO_Get(DAQ_Fifo);

        // apply running mean subtraction to remove baseline drift
        sum += sample;
        N += 1;
        sample -= sum / ((float32_t) N);

        // apply 60 [Hz] notch filter to remove power line noise
        sample = DAQ_NotchFilter(sample);

        // place in FIFO buffers
        Debug_Assert(FIFO_isFull(QRS_Fifo) == false);
        FIFO_Put(QRS_Fifo, *((uint32_t *) (&sample)));

        Debug_Assert(FIFO_isFull(LCD_Fifo) == false);
        FIFO_Put(LCD_Fifo, *((uint32_t *) (&sample)));
    }

    if(FIFO_isFull(QRS_Fifo)) {
        QRS_bufferIsFull = true;
    }
    else {
        // doesn't trigger if QRS detection is ready to start
        ISR_triggerInterrupt(LCD_VECTOR_NUM);
    }
}

static uint16_t LCD_prevSampleBuffer[LCD_X_MAX] = { 0 };

/**
 * @brief   Applies a 0.5-40 [Hz] bandpass filter and plots the sample to the waveform.
 * @details This ISR has a priority level of 1 and is triggered by the Processing ISR.
 *          This ISR also plots an intermediate sample to the display to make the waveform
 *          look more continuous.
 *
 * @pre     Initialize the LCD module.
 * @post    The bandpass-filtered sample is plotted to the LCD.
 *
 * @see     LCD_Init(), Processing_Handler()
 *
 * @callgraph
 */
static void LCD_Handler(void) {
    static uint16_t x = 0;
    static const float32_t maxVal = LOOKUP_DAQ_MAX * 2;

    Debug_Assert(FIFO_isEmpty(LCD_Fifo) == false);

    // NOTE: this `while` is only here in case a sample arrives while the QRS FIFO is being emptied
    while(FIFO_isEmpty(LCD_Fifo) == false) {

        // get sample and apply 0.5-40 [Hz] bandpass filter
        float32_t sample;
        *((uint32_t *) &sample) = FIFO_Get(LCD_Fifo);
        sample = DAQ_BandpassFilter(sample);

        // remove previous sample
        uint16_t y = LCD_prevSampleBuffer[x];
        LCD_setColor(LCD_BLACK_INV);
        LCD_setX(x, x);
        LCD_setY(y, y);
        LCD_Draw();

        // plot current sample
        y = LCD_WAVE_Y_MIN + ((uint16_t) (((sample + maxVal) / (maxVal * 2)) * LCD_WAVE_NUM_Y));
        LCD_setColor(LCD_RED_INV);
        LCD_setY(y, y);
        LCD_Draw();

        // store y-value and update x
        LCD_prevSampleBuffer[x] = y;
        x = (x + 1) % LCD_X_MAX;
    }
}

/** @} */               // main
