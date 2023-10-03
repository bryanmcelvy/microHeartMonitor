/**
 * @addtogroup daq
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for DAQ module.
 */

/******************************************************************************
Preprocessor Directives
*******************************************************************************/
#include "DAQ.h"

#include "ADC.h"
#include "Timer.h"

#include "FIFO.h"
#include "NewAssert.h"

#include "arm_math_types.h"
#include "dsp/filtering_functions.h"
#include "lookup.h"
#include "tm4c123gh6pm.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#define SAMPLING_PERIOD_MS 5               ///< sampling period in ms (\f$ T_s = 1 / f_s \f$)

/******************************************************************************
Static Declarations
*******************************************************************************/
typedef arm_biquad_casd_df1_inst_f32 filt_t;

enum {
    NUM_FILT_STAGES = 10,
    NUM_FILT_COEFFS = NUM_FILT_STAGES * 5,
    STATE_BUFF_SIZE = NUM_FILT_STAGES * 4,
};

// clang-format off
static const float32_t FILT_COEFFS[NUM_FILT_COEFFS] = {
    // Section 1
    0.2869851589202881f, 0.32466086745262146f, 0.2869851589202881f, 
    -0.20968256890773773f, -0.1729172021150589f, 
    // Section 2
    1.0f, -0.4715292155742645f, 1.0f, 
    0.5868059992790222f, -0.7193671464920044f, 
    // Section 3
    1.0f, 0.6183391213417053f, 1.0f, 
    -0.5850160717964172f, -0.9409302473068237f, 
    // Section 4
    1.0f, 0.6183391213417053f, 1.0f, 
    -0.615153431892395f, -0.9412328004837036f, 
    // Section 5
    1.0f, 0.6183391213417053f, 1.0f, 
    -0.5631667971611023f, -0.9562366008758545f, 
    // Section 6
    1.0f, 0.6183391213417053f, 1.0f, 
    -0.6460562348365784f, -0.9568508863449097f, 
    // Section 7
    1.0f, 0.6183391213417053f, 1.0f, 
    -0.5554963946342468f, -0.9837208390235901f, 
    // Section 8
    1.0f, 0.6183391213417053f, 1.0f, 
    -0.6700929999351501f, -0.9840363264083862f, 
    // Section 9
    1.0f, -1.9999638795852661f, 1.0f, 
    1.9863483905792236f, -0.986438512802124f, 
    // Section 10
    1.0f, -1.9997893571853638f, 1.0f, 
    1.994096040725708f, -0.9943605065345764f, 
};
// clang-format on
static float32_t stateBuffer[STATE_BUFF_SIZE];
static const filt_t filterStruct = { NUM_FILT_STAGES, stateBuffer, FILT_COEFFS };
static filt_t * const filter = &filterStruct;

/******************************************************************************
Functions
*******************************************************************************/
void DAQ_Init(void) {

    ADC_Init();

    Timer_t DAQ_Timer = Timer_Init(TIMER3);
    Timer_setMode(DAQ_Timer, PERIODIC, UP);
    Timer_enableAdcTrigger(DAQ_Timer);
    Timer_setInterval_ms(DAQ_Timer, SAMPLING_PERIOD_MS);
    Timer_Start(DAQ_Timer);

    return;
}

float32_t DAQ_Filter(volatile float32_t inputSample) {
    float32_t * inputPtr = &inputSample;

    float32_t outputSample = 0;
    float32_t * outputPtr = &outputSample;

    arm_biquad_cascade_df1_f32(filter, inputPtr, outputPtr, 1);
    Assert(isinf(outputSample) == false);

    return outputSample;
}

/** @} */
