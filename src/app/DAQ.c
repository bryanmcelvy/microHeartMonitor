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
    // Input Filter: 2nd Order Low Pass Filter (fc = 40 [Hz])
    NUM_STAGES_LOWPASS = 2,
    NUM_COEFF_INPUT = NUM_STAGES_LOWPASS * 5,
    STATE_BUFF_SIZE_LOWPASS = NUM_STAGES_LOWPASS * 4,

    // Notch Filter: 1st Order 60 [Hz] Bandstop Filter
    NUM_STAGES_NOTCH = 2,
    NUM_COEFF_NOTCH = NUM_STAGES_NOTCH * 5,
    STATE_BUFF_SIZE_NOTCH = NUM_STAGES_NOTCH * 4
};

static const float32_t FILT_COEFF_LOWPASS[NUM_COEFF_INPUT] = {
    0.046582907f, 0.093165815f, 0.04658291f, -0.32897568f, 0.06458765f,
    1.0f,         2.0f,         0.99999994f, -0.45311952f, 0.4663256f
};

// TODO: Fix notch filter
static const float32_t FILT_COEFF_NOTCH[NUM_COEFF_NOTCH] = {
    0.9565432255568765f, 0.5914680788951381f, 0.9565432255568764f,
    0.5631667954285263f, 0.9562366248653332f, 1.0f,
    0.618339101770126f,  0.9999999999999999f, 0.6460562453305682f,
    0.9568508578410597f
};

static filt_t lowPassFiltStruct = { 0 };
static filt_t * const lowPassFilter = &lowPassFiltStruct;
static float32_t stateBuffer_LowPass[STATE_BUFF_SIZE_LOWPASS];

static filt_t notchFiltStruct = { 0 };
static filt_t * const notchFilter = &notchFiltStruct;
static float32_t stateBuffer_Notch[STATE_BUFF_SIZE_NOTCH];

/******************************************************************************
Functions
*******************************************************************************/
void DAQ_Init(void) {

    arm_biquad_cascade_df1_init_f32(lowPassFilter, NUM_STAGES_LOWPASS, FILT_COEFF_LOWPASS,
                                    stateBuffer_LowPass);
    arm_biquad_cascade_df1_init_f32(notchFilter, NUM_STAGES_NOTCH, FILT_COEFF_NOTCH,
                                    stateBuffer_Notch);

    ADC_Init();
    Timer3A_Init(SAMPLING_PERIOD_MS);

    return;
}

float32_t DAQ_Filter(volatile float32_t inputSample) {
    float32_t * inputPtr = &inputSample;

    // Low Pass Filter
    float32_t outputSample = 0;
    float32_t * outputPtr = &outputSample;
    arm_biquad_cascade_df1_f32(lowPassFilter, inputPtr, outputPtr, 1);
    Assert(isinf(outputSample) == false);

    // Notch Filter
    float32_t tmp = outputSample;
    float32_t * tmpPtr = &tmp;
    arm_biquad_cascade_df1_f32(notchFilter, tmpPtr, outputPtr, 1);
    Assert(isinf(outputSample) == false);
    // Assert((outputSample > (LOOKUP_ADC_MIN * 4)) && (outputSample < (LOOKUP_ADC_MAX * 4)));

    return outputSample;
}

/** @} */
