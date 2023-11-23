/**
 * @addtogroup main
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Main program file (bare-metal implementation).
 */

/******************************************************************************
SECTIONS
        Direct Dependencies
        Function Declarations
        Variable Declarations
        Function Definitions
******************************************************************************/

/******************************************************************************
Direct Dependencies
******************************************************************************/

// application-specific
#include "DAQ.h"
#include "LCD.h"
#include "QRS.h"

// middleware
#include "Debug.h"

// common
#include "FIFO.h"

// drivers
#include "GPIO.h"
#include "ISR.h"
#include "PLL.h"
#include "UART.h"

// vendor (i.e. external/device) files
#include "arm_math_types.h"
#include "tm4c123gh6pm.h"

// other
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
Function Declarations
******************************************************************************/

enum ISR_VECTOR_NUMS {
    DAQ_VECTOR_NUM = INT_ADC0SS3,               ///< vector number for the @ref DAQ_Handler()
    PROC_VECTOR_NUM = INT_CAN0,                 ///< vector number for the @ref Processing_Handler()
    LCD_VECTOR_NUM = INT_TIMER1A                ///< vector number for the @ref LCD_Handler()
};

/**
 * @brief   ISR for the data acquisition system.
 *
 * @details This ISR has a priority level of 1, is triggered when the ADC has finished capturing
 *          a sample, and also triggers the intermediate processing handler. It reads the 12-bit
 *          ADC output, converts it from an integer to a raw voltage sample, and sends it to the
 *          processing ISR via the @ref DAQ_Fifo.
 *
 * @pre     Initialize the DAQ module.
 * @post    The converted sample is placed in the DAQ FIFO, and the processing ISR is triggered.
 *
 * @see     DAQ_Init(), Processing_Handler()
 *
 * @image   html software/main3_daq.png "Flowchart for the DAQ handler." width=150cm
 * @image   latex software/main3_daq.png "Flowchart for the DAQ handler."
 * @callgraph
 */
static void DAQ_Handler(void);

/**
 * @brief   ISR for intermediate processing of the input data.
 *
 * @details This ISR has a priority level of 1, is triggered by the DAQ ISR, and triggers the LCD
 *          handler. It removes baseline drift and power line interference (PLI) from a sample, and
 *          then moves it to the @ref QRS_Fifo and the @ref LCD_Fifo. It also notifies the superloop
 *          in @ref main() when the QRS buffer is full.
 *
 * @post    The converted sample is placed in the LCD FIFO, and the LCD ISR is triggered.
 * @post    The converted sample is placed in the QRS FIFO, and the flag is set.
 *
 * @see     DAQ_Handler(), main(), LCD_Handler()
 *
 * @image   html software/main4_proc.png "Flowchart for the processing handler." width=350cm
 * @image   latex software/main4_proc.png "Flowchart for the processing handler."
 * @callgraph
 */
static void Processing_Handler(void);

/**
 * @brief   ISR for plotting the waveform and outputting the heart rate to the LCD.
 *
 * @details This ISR has a priority level of 1 and is triggered by the Processing ISR. It applies a
 *          0.5-40 [Hz] bandpass filter to the sample and plots it. It also outputs the heart rate.
 *
 * @pre     Initialize the LCD module.
 * @post    The bandpass-filtered sample is plotted to the LCD.
 * @post    The heart rate is updated after each block is analyzed.
 *
 * @see     LCD_Init(), Processing_Handler(), main()
 *
 * @image   html software/main5_lcd.png "Flowchart for the LCD handler." width=350cm
 * @image   latex software/main5_lcd.png "Flowchart for the LCD handler."
 * @callgraph
 */
static void LCD_Handler(void);

/******************************************************************************
Variable Declarations
******************************************************************************/

enum FIFO_INFO {
    DAQ_FIFO_CAP = 3,                                   ///< capacity of DAQ's FIFO buffer
    DAQ_ARRAY_LEN = DAQ_FIFO_CAP + 1,                   ///< actual size of underlying array

    QRS_FIFO_CAP = QRS_NUM_SAMP,                        ///< capacity of QRS detector's FIFO buffer
    QRS_ARRAY_LEN = QRS_FIFO_CAP + 1,                   ///< actual size of underlying array

    LCD_FIFO_1_CAP = DAQ_FIFO_CAP,                      ///< capacity of LCD's waveform FIFO buffer
    LCD_ARRAY_1_LEN = LCD_FIFO_1_CAP + 1,               ///< actual size of underlying array

    LCD_FIFO_2_CAP = 1,                                ///< capacity of LCD's heart rate FIFO buffer
    LCD_ARRAY_2_LEN = LCD_FIFO_2_CAP + 1               ///< actual size of underlying array
};

static volatile Fifo_t DAQ_Fifo = 0;
static volatile uint32_t DAQ_fifoBuffer[DAQ_ARRAY_LEN] = { 0 };

static volatile Fifo_t QRS_Fifo = 0;
static volatile uint32_t QRS_fifoBuffer[QRS_ARRAY_LEN] = { 0 };

static volatile Fifo_t LCD_Fifo1 = 0;
static volatile uint32_t LCD_fifoBuffer1[LCD_ARRAY_1_LEN] = { 0 };

static volatile Fifo_t LCD_Fifo2 = 0;
static volatile uint32_t LCD_fifoBuffer2[LCD_ARRAY_2_LEN] = { 0 };

static volatile bool qrsBufferIsFull = false;                ///< flag for QRS detection to start
static volatile bool heartRateIsReady = false;               ///< flag for LCD to output heart rate

// NOTE: using just one processing buffer to avoid running out of RAM
static float32_t QRS_processingBuffer[QRS_ARRAY_LEN] = { 0 };

enum LCD_INFO {
    LCD_TOP_LINE = (LCD_Y_MAX - 24),               ///< separates wavefrom from text

    LCD_WAVE_NUM_Y = LCD_TOP_LINE,               ///< num. of y-vals available for plotting waveform
    LCD_WAVE_X_OFFSET = 0,                       ///< waveform's offset from X axis
    LCD_WAVE_Y_MIN = (0 + LCD_WAVE_X_OFFSET),                            ///< waveform's min y-value
    LCD_WAVE_Y_MAX = (LCD_WAVE_NUM_Y + LCD_WAVE_X_OFFSET),               ///< waveform's max y-value

    LCD_TEXT_LINE_NUM = 28,                                              ///< line num. of text
    LCD_TEXT_COL_NUM = 24               ///< starting col. num. for heart rate
};

static uint16_t LCD_prevSampleBuffer[LCD_X_MAX] = { 0 };

/******************************************************************************
Function Definitions
******************************************************************************/

/**
 * @brief           Main function for the project.
 * @details         Moves the interrupt vector table to RAM; configures and
 *                  enables the ISRs; initializes all modules and static variables;
 *                  and performs QRS detection once the buffer has been filled.
 *
 * @image html software/main1_init.png "Flowchart for the initialization phase."  width=150cm
 * @image latex software/main1_init.png "Flowchart for the initialization phase."
 * @image html software/main2_superloop.png "Flowchart for the superloop."  width=150cm
 * @image latex software/main2_superloop.png "Flowchart for the superloop."
 * @callgraph
 */
int main(void) {
    static GpioPort_t portA = 0;
    static Uart_t uart0 = 0;

    PLL_Init();

    // Init. debug module
    portA = GPIO_InitPort(GPIO_PORT_A);
    uart0 = UART_Init(portA, UART0);
    Debug_Init(uart0);

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
    DAQ_Fifo = Fifo_Init(DAQ_fifoBuffer, DAQ_ARRAY_LEN);
    QRS_Fifo = Fifo_Init(QRS_fifoBuffer, QRS_ARRAY_LEN);
    LCD_Fifo1 = Fifo_Init(LCD_fifoBuffer1, LCD_ARRAY_1_LEN);
    LCD_Fifo2 = Fifo_Init(LCD_fifoBuffer2, LCD_ARRAY_2_LEN);

    // Init./config. LCD
    LCD_Init();
    LCD_setOutputMode(false);

    LCD_setColor(LCD_WHITE);
    LCD_drawHoriLine(LCD_TOP_LINE, 1);

    LCD_setColor(LCD_RED);
    LCD_setCursor(LCD_TEXT_LINE_NUM, 0);
    LCD_writeStr("Heart Rate:      bpm");

    LCD_setOutputMode(true);

    Debug_SendFromList(DEBUG_LCD_INIT);

    // Init. other app. modules
    QRS_Init();
    Debug_SendFromList(DEBUG_QRS_INIT);

    DAQ_Init();
    Debug_SendFromList(DEBUG_DAQ_INIT);

    // Enable interrupts and start
    ISR_GlobalEnable();
    while(1) {
        if(qrsBufferIsFull) {               // flag set by Processing_Handler()
            // Transfer samples from FIFO
            ISR_Disable(PROC_VECTOR_NUM);

            Fifo_FlushFloat(QRS_Fifo, QRS_processingBuffer);
            qrsBufferIsFull = false;

            ISR_Enable(PROC_VECTOR_NUM);

            // Run QRS detection
            Debug_SendMsg("Starting QRS detection...\r\n");

            QRS_Preprocess(QRS_processingBuffer, QRS_processingBuffer);
            float32_t heartRate_bpm = QRS_applyDecisionRules(QRS_processingBuffer);
            Debug_Assert(isfinite(heartRate_bpm));

            // Output heart rate to serial port
            Debug_WriteFloat(heartRate_bpm);

            // Output heart rate to LCD
            Fifo_PutFloat(LCD_Fifo2, heartRate_bpm);
            heartRateIsReady = true;
        }
    }
}

static void DAQ_Handler(void) {
    // read sample and convert to `float32_t`
    uint16_t rawSample = DAQ_readSample();
    volatile float32_t sample = DAQ_convertToMilliVolts(rawSample);

    // send to intermediate processing handler
    Debug_Assert(Fifo_isFull(DAQ_Fifo) == false);
    Fifo_PutFloat(DAQ_Fifo, sample);
    ISR_triggerInterrupt(PROC_VECTOR_NUM);

    DAQ_acknowledgeInterrupt();
}

static void Processing_Handler(void) {
    static float32_t sum = 0;
    static uint32_t N = 0;

    // NOTE: this `while` is only here in case a sample arrives while the QRS FIFO is being emptied
    while(Fifo_isEmpty(DAQ_Fifo) == false) {
        volatile float32_t sample = Fifo_GetFloat(DAQ_Fifo);

        // apply running mean subtraction to remove baseline drift
        sum += sample;
        N += 1;
        sample -= sum / ((float32_t) N);

        // apply 60 [Hz] notch filter to remove power line noise
        sample = DAQ_NotchFilter(sample);

        // place in FIFO buffers
        Debug_Assert(Fifo_isFull(QRS_Fifo) == false);
        Fifo_PutFloat(QRS_Fifo, sample);

        Debug_Assert(Fifo_isFull(LCD_Fifo1) == false);
        Fifo_PutFloat(LCD_Fifo1, sample);
    }

    if(Fifo_isFull(QRS_Fifo)) {
        qrsBufferIsFull = true;
    }
    else {
        // doesn't trigger if QRS detection is ready to start
        ISR_triggerInterrupt(LCD_VECTOR_NUM);
    }
}

static void LCD_Handler(void) {
    static uint16_t x = 0;
    static const float32_t maxVal = DAQ_LOOKUP_MAX * 2;

    Debug_Assert(Fifo_isEmpty(LCD_Fifo1) == false);

    // NOTE: this `while` is only here in case a sample arrives while the QRS FIFO is being emptied
    while(Fifo_isEmpty(LCD_Fifo1) == false) {
        // get sample and apply 0.5-40 [Hz] bandpass filter
        float32_t sample = Fifo_GetFloat(LCD_Fifo1);
        sample = DAQ_BandpassFilter(sample);

        // remove previous y-value from LCD
        uint16_t y = LCD_prevSampleBuffer[x];
        LCD_plotSample(x, y, LCD_BLACK);

        // shift/scale `sample` from (est.) range [-11, 11) to [LCD_WAVE_Y_MIN, LCD_WAVE_Y_MAX)
        y = LCD_WAVE_Y_MIN + ((uint16_t) (((sample + maxVal) / (maxVal * 2)) * LCD_WAVE_Y_MAX));
        LCD_plotSample(x, y, LCD_RED);

        // store y-value and update x
        LCD_prevSampleBuffer[x] = y;
        x = (x + 1) % LCD_X_MAX;
    }

    if(heartRateIsReady) {
        volatile float32_t heartRate_bpm = Fifo_GetFloat(LCD_Fifo2);

        LCD_setCursor(LCD_TEXT_LINE_NUM, LCD_TEXT_COL_NUM);
        LCD_writeFloat(heartRate_bpm);

        heartRateIsReady = false;
    }
}

/** @} */               // main
