/**
 * @file
 * @author  Bryan McElvy
 * @brief   Main program file (RTOS implementation).
 */

/******************************************************************************
SECTIONS
        Direct Dependencies
        FreeRTOS Task Declarations
        FreeRTOS Queue Declarations
        Other Declarations
        Main Function Definition
        ISR/Task Definitions
******************************************************************************/

/******************************************************************************
Direct Dependencies
******************************************************************************/

// FreeRTOS
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

// application-specific
#include "DAQ.h"
#include "LCD.h"
#include "QRS.h"

// middleware
#include "Debug.h"

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
FreeRTOS Task Declarations
******************************************************************************/

#define STACK_SIZE ((UBaseType_t) 100)

enum TASK_PRIORITIES {
    PROC_TASK_PRI = 2,
    QRS_TASK_PRI = 3,
    LCD_WAVEFORM_TASK_PRI = 2,
    LCD_HR_TASK_PRI = LCD_WAVEFORM_TASK_PRI,
};

static TaskHandle_t ProcessingTaskHandle = 0;
static void ProcessingTask(void * params);
static StackType_t ProcessingStack[STACK_SIZE] = { 0 };
static StaticTask_t ProcessingTaskBuffer;

static TaskHandle_t QrsDetectionTaskHandle = 0;
static void QrsDetectionTask(void * params);
static StackType_t QrsDetectionStack[STACK_SIZE] = { 0 };
static StaticTask_t QrsDetectionTaskBuffer;

static TaskHandle_t LcdWaveformTaskHandle = 0;
static void LcdWaveformTask(void * params);
static StackType_t LcdWaveformStack[STACK_SIZE] = { 0 };
static StaticTask_t LcdWaveformTaskBuffer;

static TaskHandle_t LcdHeartRateTaskHandle = 0;
static void LcdHeartRateTask(void * params);
static StackType_t LcdHeartRateStack[STACK_SIZE] = { 0 };
static StaticTask_t LcdHeartRateTaskBuffer;

/******************************************************************************
FreeRTOS Queue Declarations
******************************************************************************/

enum QUEUE_INFO {
    QUEUE_ITEM_SIZE = sizeof(uint32_t),               ///< size in bytes for each queue

    DAQ_2_PROC_LEN = 3,                               ///< length of DAQ-to-Processing task queue
    PROC_2_QRS_LEN = QRS_NUM_SAMP,                    ///< length of Processing-to-QRS task queue
    PROC_2_LCD_LEN = DAQ_2_PROC_LEN,                  ///< length of Processing-to-LCD task queue
    QRS_2_LCD_LEN = 1,                                ///< length of QRS-to-LCD task queue
};

QueueHandle_t Daq2ProcQueue = 0;
StaticQueue_t Daq2ProcQueueBuffer = { 0 };
uint8_t Daq2ProcQueueStorageArea[DAQ_2_PROC_LEN * QUEUE_ITEM_SIZE] = { 0 };

QueueHandle_t Proc2QrsQueue = 0;
StaticQueue_t Proc2QrsQueueBuffer = { 0 };
uint8_t Proc2QrsQueueStorageArea[PROC_2_QRS_LEN * QUEUE_ITEM_SIZE] = { 0 };

QueueHandle_t Proc2LcdQueue = 0;
StaticQueue_t Proc2LcdQueueBuffer = { 0 };
uint8_t Proc2LcdQueueStorageArea[PROC_2_LCD_LEN * QUEUE_ITEM_SIZE] = { 0 };

QueueHandle_t Qrs2LcdQueue = 0;
StaticQueue_t Qrs2LcdQueueBuffer;
uint8_t Qrs2LcdQueueStorageArea[QRS_2_LCD_LEN * QUEUE_ITEM_SIZE] = { 0 };

/******************************************************************************
Other Declarations
******************************************************************************/

#define Daq_Handler    ADC0_SS3_Handler
#define DAQ_VECTOR_NUM INT_ADC0SS3

static float32_t qrsDetectionBuffer[QRS_NUM_SAMP] = { 0 };

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
Main Function Definition
******************************************************************************/

int main(void) {
    static GpioPort_t portA = 0;
    static Uart_t uart0 = 0;

    PLL_Init();

    // Init. debug module
    portA = GPIO_InitPort(GPIO_PORT_A);
    uart0 = UART_Init(portA, UART0);
    Debug_Init(uart0);

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

    // Init. DAQ ISR
    ISR_GlobalDisable();
    ISR_setPriority(DAQ_VECTOR_NUM, 1);
    ISR_Enable(DAQ_VECTOR_NUM);
    ISR_GlobalEnable();

    // Init. queues and add them to registry for debugging
    Daq2ProcQueue = xQueueCreateStatic(DAQ_2_PROC_LEN, QUEUE_ITEM_SIZE, Daq2ProcQueueStorageArea,
                                       &Daq2ProcQueueBuffer);
    vQueueAddToRegistry(Daq2ProcQueue, "DAQ-to-Processing Task Queue");
    Proc2QrsQueue = xQueueCreateStatic(PROC_2_QRS_LEN, QUEUE_ITEM_SIZE, Proc2QrsQueueStorageArea,
                                       &Proc2QrsQueueBuffer);
    Proc2LcdQueue = xQueueCreateStatic(PROC_2_LCD_LEN, QUEUE_ITEM_SIZE, Proc2LcdQueueStorageArea,
                                       &Proc2LcdQueueBuffer);
    Qrs2LcdQueue = xQueueCreateStatic(QRS_2_LCD_LEN, QUEUE_ITEM_SIZE, Qrs2LcdQueueStorageArea,
                                      &Qrs2LcdQueueBuffer);

    // Init. tasks and start scheduler
    ProcessingTaskHandle =
        xTaskCreateStatic(ProcessingTask, "Intermediate Processing", STACK_SIZE, NULL,
                          PROC_TASK_PRI, ProcessingStack, &ProcessingTaskBuffer);
    QrsDetectionTaskHandle =
        xTaskCreateStatic(QrsDetectionTask, "QRS Detection", STACK_SIZE, NULL, QRS_TASK_PRI,
                          QrsDetectionStack, &QrsDetectionTaskBuffer);
    LcdWaveformTaskHandle =
        xTaskCreateStatic(LcdWaveformTask, "LCD (Waveform)", STACK_SIZE, NULL,
                          LCD_WAVEFORM_TASK_PRI, LcdWaveformStack, &LcdWaveformTaskBuffer);
    LcdHeartRateTaskHandle =
        xTaskCreateStatic(LcdHeartRateTask, "LCD (Heart Rate)", STACK_SIZE, NULL, LCD_HR_TASK_PRI,
                          LcdHeartRateStack, &LcdHeartRateTaskBuffer);

    vTaskStartScheduler();
    while(1) {}
}

/******************************************************************************
ISR/Task Definitions
******************************************************************************/

void Daq_Handler(void) {
    // read sample and convert to `float32_t`
    uint16_t rawSample = DAQ_readSample();
    volatile float32_t sample = DAQ_convertToMilliVolts(rawSample);

    // send to intermediate processing task
    BaseType_t status = xQueueSendToBackFromISR(Daq2ProcQueue, &sample, NULL);
    Debug_Assert(status == pdTRUE);

    // acknowledge interrupt and unsuspend processing task
    DAQ_acknowledgeInterrupt();
    BaseType_t xYieldRequired = xTaskResumeFromISR(ProcessingTaskHandle);
    portYIELD_FROM_ISR(xYieldRequired);
}

static void ProcessingTask(void * params) {
    while(1) {
        static float32_t sum = 0;
        static uint32_t N = 0;

        // process sample(s) and place in queues
        while(uxQueueMessagesWaiting(Daq2ProcQueue) > 0) {
            volatile float32_t sample;
            xQueueReceive(Daq2ProcQueue, &sample, 0);

            // apply running mean subtraction to remove baseline drift
            sum += sample;
            N += 1;
            sample -= sum / ((float32_t) N);

            // apply 60 [Hz] notch filter to remove power line noise
            sample = DAQ_NotchFilter(sample);

            // place in queues
            BaseType_t status;

            status = xQueueSendToBack(Proc2QrsQueue, &sample, 0);
            Debug_Assert(status == pdTRUE);

            status = xQueueSendToBack(Proc2LcdQueue, &sample, 0);
            Debug_Assert(status == pdTRUE);
        }

        // activate next task(s) and suspend itself
        if(uxQueueSpacesAvailable(Proc2QrsQueue) == pdFALSE) {
            vTaskResume(QrsDetectionTaskHandle);
        }
        vTaskResume(LcdWaveformTaskHandle);
        vTaskSuspend(NULL);
    }
}

static void QrsDetectionTask(void * params) {
    while(1) {
        // disable processing task and flush queue into QRS detection buffer
        vTaskPrioritySet(ProcessingTaskHandle, configMAX_PRIORITIES - 1);
        for(uint16_t idx = 0; idx < QRS_NUM_SAMP; idx++) {
            xQueueReceive(Proc2QrsQueue, &qrsDetectionBuffer[idx], 0);
        }
        vTaskPrioritySet(ProcessingTaskHandle, PROC_TASK_PRI);

        // Run QRS detection
        Debug_SendMsg("Starting QRS detection...\r\n");

        QRS_Preprocess(qrsDetectionBuffer, qrsDetectionBuffer);
        float32_t heartRate_bpm = QRS_applyDecisionRules(qrsDetectionBuffer);
        Debug_Assert(isfinite(heartRate_bpm));

        // Output heart rate to serial port
        Debug_WriteFloat(heartRate_bpm);

        // Output heart rate to LCD
        xQueueSendToBack(Qrs2LcdQueue, &heartRate_bpm, 0);
        vTaskResume(LcdHeartRateTaskHandle);

        vTaskSuspend(NULL);
    }
}

static void LcdWaveformTask(void * params) {
    while(1) {
        static uint16_t x = 0;
        static const float32_t maxVal = DAQ_LOOKUP_MAX * 2;

        while(uxQueueMessagesWaiting(Proc2LcdQueue) > 0) {
            float32_t sample;
            xQueueReceive(Proc2LcdQueue, &sample, 0);
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

        vTaskSuspend(NULL);
    }
}

static void LcdHeartRateTask(void * params) {
    while(1) {
        volatile float32_t heartRate_bpm;
        xQueueReceive(Qrs2LcdQueue, &heartRate_bpm, 0);

        LCD_setCursor(LCD_TEXT_LINE_NUM, LCD_TEXT_COL_NUM);
        LCD_writeStr((void *) "     ");               // 5 spaces

        LCD_setCursor(LCD_TEXT_LINE_NUM, LCD_TEXT_COL_NUM);
        LCD_writeFloat(heartRate_bpm);

        vTaskSuspend(NULL);
    }
}

/** @} */                                             // main
