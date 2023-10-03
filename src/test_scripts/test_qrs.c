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

// Interrupt Service Routines (ISRs)
enum {               // clang-format off
    ADC_VECTOR_NUM = INT_ADC0SS3,
    DAQ_VECTOR_NUM = INT_CAN0,
    QRS_VECTOR_NUM = INT_ADC0SS0
};               // clang-format on

static void ADC_Handler(void);
static void DAQ_Handler(void);
static void QRS_Handler(void);

// FIFO Buffers
enum {
    DAQ_BUFFER_CAPACITY = 8,
    DAQ_BUFFER_SIZE = DAQ_BUFFER_CAPACITY + 1,
    QRS_BUFFER_SIZE = QRS_NUM_SAMP + 1
};

static volatile FIFO_t * DAQ_Fifo = 0;
static volatile uint32_t DAQ_Buffer[DAQ_BUFFER_SIZE] = { 0 };

static volatile FIFO_t * QRS_Fifo = 0;
static volatile uint32_t QRS_FifoBuffer[QRS_BUFFER_SIZE] = { 0 };
static volatile bool QRS_bufferIsFull = false;

// Processing Buffers
volatile float32_t QRS_InputBuffer[QRS_BUFFER_SIZE] = { 0 };
volatile float32_t QRS_OutputBuffer[QRS_BUFFER_SIZE] = { 0 };

/******************************************************************************
Main
******************************************************************************/

int main(void) {
    PLL_Init();
    Debug_Init();

    // Init. vector table and ISRs
    ISR_GlobalDisable();
    ISR_InitNewTableInRam();

    ISR_addToIntTable(ADC_Handler, ADC_VECTOR_NUM);
    ISR_setPriority(ADC_VECTOR_NUM, 2);
    ISR_Enable(ADC_VECTOR_NUM);

    ISR_addToIntTable(DAQ_Handler, DAQ_VECTOR_NUM);
    ISR_setPriority(DAQ_VECTOR_NUM, 2);
    ISR_Enable(DAQ_VECTOR_NUM);

    ISR_addToIntTable(QRS_Handler, QRS_VECTOR_NUM);
    ISR_setPriority(QRS_VECTOR_NUM, 1);
    ISR_Enable(QRS_VECTOR_NUM);

    // Init. FIFOs
    DAQ_Fifo = FIFO_Init(DAQ_Buffer, DAQ_BUFFER_SIZE);
    QRS_Fifo = FIFO_Init(QRS_FifoBuffer, QRS_BUFFER_SIZE);

    // Init. app. modules
    DAQ_Init();
    Debug_SendFromList(DEBUG_DAQ_INIT);

    QRS_Init();
    Debug_SendFromList(DEBUG_QRS_INIT);

    ISR_GlobalEnable();
    while(1) {
        if(QRS_bufferIsFull == true) {
            ISR_Disable(DAQ_VECTOR_NUM);

            FIFO_Flush(QRS_Fifo, (uint32_t *) QRS_InputBuffer);
            QRS_bufferIsFull = false;

            ISR_Enable(DAQ_VECTOR_NUM);
            ISR_triggerInterrupt(QRS_VECTOR_NUM);

            float32_t heartRate_bpm = QRS_ApplyDecisionRules(QRS_OutputBuffer);
            Debug_Assert(isnan(heartRate_bpm) == false);
            Debug_Assert(isinf(heartRate_bpm) == false);
            Debug_WriteFloat(heartRate_bpm);
        }
    }
}

/******************************************************************************
Interrupt Service Routines
******************************************************************************/

static void ADC_Handler(void) {
    Debug_Assert(FIFO_isFull(DAQ_Fifo) == false);
    FIFO_Put(DAQ_Fifo, (volatile uint32_t)(ADC0_SSFIFO3_R & 0xFFF));

    ADC0_ISC_R |= 0x08;               // clear interrupt flag to acknowledge
    ISR_triggerInterrupt(DAQ_VECTOR_NUM);
}

static void DAQ_Handler(void) {
    while(FIFO_isEmpty(DAQ_Fifo) == false) {
        volatile uint16_t raw_sample = FIFO_Get(DAQ_Fifo);
        volatile float32_t sample = ADC_ConvertToVolts(raw_sample);
        sample = DAQ_Filter(sample);
        Debug_Assert(isnan(sample) == false);
        Debug_Assert(isinf(sample) == false);

        FIFO_Put(QRS_Fifo, *((uint32_t *) (&sample)));
        if(FIFO_isFull(QRS_Fifo)) {
            QRS_bufferIsFull = true;
        }
    }
}

static void QRS_Handler(void) {
    Debug_SendMsg("Starting QRS detection...\r\n");

    QRS_Preprocess(QRS_InputBuffer, QRS_OutputBuffer);
}
