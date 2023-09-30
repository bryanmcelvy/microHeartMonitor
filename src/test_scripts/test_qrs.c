/**
 * @file
 * @author  Bryan McElvy
 * @brief   QRS detector test script.
 */

/******************************************************************************
Preprocessor Directives
******************************************************************************/

// Application Software
#include "DAQ.h"
#include "Debug.h"
#include "QRS.h"

// Drivers
#include "ADC.h"
#include "PLL.h"

// Common
#include "FIFO.h"
#include "ISR.h"
#include "lookup.h"

// Other
#include "arm_math_types.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
Declarations
******************************************************************************/

static void DAQ_Handler(void);
const uint8_t DAQ_VECTOR_NUM = INT_CAN0;

enum {
    DAQ_BUFFER_CAPACITY = 16,
    DAQ_BUFFER_SIZE = DAQ_BUFFER_CAPACITY + 1,
    QRS_BUFFER_SIZE = QRS_NUM_SAMP + 1
};

volatile FIFO_t * DAQ_Fifo = 0;
volatile uint32_t DAQ_Buffer[DAQ_BUFFER_SIZE] = { 0 };
volatile bool sampleReady = false;

FIFO_t * QRS_Fifo = 0;
float32_t QRS_FifoBuffer[QRS_BUFFER_SIZE] = { 0 };
float32_t QRS_InputBuffer[QRS_NUM_SAMP] = { 0 };
float32_t QRS_OutputBuffer[QRS_NUM_SAMP] = { 0 };

/******************************************************************************
Main
******************************************************************************/

int main(void) {
    PLL_Init();
    Debug_Init();

    ISR_GlobalDisable();
    ISR_InitNewTableInRam();

    QRS_Fifo = FIFO_Init(QRS_FifoBuffer, QRS_BUFFER_SIZE);
    QRS_Init();
    Debug_SendFromList(DEBUG_QRS_INIT);

    DAQ_Fifo = FIFO_Init(DAQ_Buffer, DAQ_BUFFER_SIZE);
    DAQ_Init();
    ISR_addToIntTable(DAQ_Handler, DAQ_VECTOR_NUM);
    ISR_setPriority(DAQ_VECTOR_NUM, 1);
    ISR_Enable(DAQ_VECTOR_NUM);
    Debug_SendFromList(DEBUG_DAQ_INIT);

    ISR_GlobalEnable();
    while(1) {
        if(FIFO_isFull(QRS_Fifo)) {
            FIFO_Flush(QRS_Fifo, QRS_InputBuffer);

            Debug_SendMsg("Starting QRS detection...");

            QRS_Preprocess(QRS_InputBuffer, QRS_OutputBuffer);
            float32_t heartRate_bpm = QRS_ApplyDecisionRules(QRS_OutputBuffer);
            Debug_Assert(isnan(heartRate_bpm) == false);
            Debug_Assert(isinf(heartRate_bpm) == false);
            Debug_WriteFloat(heartRate_bpm);
        }
    }
}

/******************************************************************************
ISRs
******************************************************************************/

void ADC0_SS3_Handler(void) {
    Debug_Assert(FIFO_isFull(DAQ_Fifo) == false);
    FIFO_Put(DAQ_Fifo, (volatile uint32_t)(ADC0_SSFIFO3_R & 0xFFF));

    ADC0_ISC_R |= 0x08;               // clear interrupt flag to acknowledge
    ISR_triggerInterrupt(DAQ_VECTOR_NUM);
}

void DAQ_Handler(void) {
    //...
    volatile uint16_t raw_sample = FIFO_Get(DAQ_Fifo);
    volatile float32_t sample = ADC_ConvertToVolts(raw_sample);
    sample = DAQ_Filter(sample);
    FIFO_Put(QRS_Fifo, sample);
}
