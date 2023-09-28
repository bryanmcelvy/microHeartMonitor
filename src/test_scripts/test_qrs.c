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
#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
Declarations
******************************************************************************/

enum {
    DAQ_BUFFER_SIZE = 64,
};

volatile FIFO_t * DAQ_Fifo = 0;
volatile uint32_t DAQ_Buffer[DAQ_BUFFER_SIZE] = { 0 };
volatile bool sampleReady = false;

/******************************************************************************
Main
******************************************************************************/

int main(void) {
    float32_t QRS_FifoBuffer[QRS_NUM_SAMP + 1] = { 0 };
    float32_t QRS_InputBuffer[QRS_NUM_SAMP] = { 0 };
    float32_t QRS_OutputBuffer[QRS_NUM_SAMP] = { 0 };

    ISR_GlobalDisable();

    PLL_Init();
    Debug_Init();

    DAQ_Fifo = FIFO_Init(DAQ_Buffer, DAQ_BUFFER_SIZE);
    DAQ_Init();
    Debug_SendFromList(DEBUG_DAQ_INIT);

    FIFO_t * QRS_Fifo = FIFO_Init(QRS_FifoBuffer, QRS_NUM_SAMP + 1);
    QRS_Init();
    Debug_SendFromList(DEBUG_QRS_INIT);

    ISR_GlobalEnable();
    while(1) {
        while(sampleReady == false) {}

        // collect sample
        volatile uint16_t raw_sample = FIFO_Get(DAQ_Fifo);
        sampleReady = !FIFO_isEmpty(DAQ_Fifo);

        // convert and filter
        volatile float32_t sample = ADC_ConvertToVolts(raw_sample);
        sample = DAQ_Filter(sample);

        // QRS detection
        FIFO_Put(QRS_Fifo, sample);
        if(FIFO_isFull(QRS_Fifo)) {
            FIFO_Flush(QRS_Fifo, QRS_InputBuffer);
            float32_t heartRate_bpm = QRS_RunDetection(QRS_InputBuffer, QRS_OutputBuffer);
            Debug_WriteFloat(heartRate_bpm);
        }
    }
}

/******************************************************************************
ADC ISR
******************************************************************************/

void ADC0_SS3_Handler(void) {
    Debug_Assert(FIFO_isFull(DAQ_Fifo) == false);
    FIFO_Put(DAQ_Fifo, (volatile uint32_t)(ADC0_SSFIFO3_R & 0xFFF));
    sampleReady = true;

    ADC0_ISC_R |= 0x08;               // clear interrupt flag to acknowledge
    return;
}
