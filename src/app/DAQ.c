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
    NUM_STAGES_LOWPASS = 4,
    NUM_COEFF_LOWPASS = NUM_STAGES_LOWPASS * 5,
    STATE_BUFF_SIZE_LOWPASS = NUM_STAGES_LOWPASS * 4,

    // Notch Filter: 1st Order 60 [Hz] Bandstop Filter
    NUM_STAGES_NOTCH = 6,
    NUM_COEFF_NOTCH = NUM_STAGES_NOTCH * 5,
    STATE_BUFF_SIZE_NOTCH = NUM_STAGES_NOTCH * 4
};

// clang-format off
static const float32_t FILT_COEFF_LOWPASS[NUM_COEFF_LOWPASS] = {
    0.33315151929855347f, 0.5766809582710266f, 0.33315151929855347f, 
    -0.9251470565795898f, -0.3086468279361725f, 
    1.0f, 0.5240882635116577f, 1.0f, 
    -0.2858136296272278f, -0.5249051451683044f, 
    1.0f, -0.2681707739830017f, 1.0f, 
    0.29658612608909607f, -0.745910108089447f, 
    1.0f, -0.5827286839485168f, 1.0f, 
    0.6055136919021606f, -0.9191300272941589f, 
};

static const float32_t FILT_COEFF_NOTCH[NUM_COEFF_NOTCH] = {
    0.8856732845306396f, 0.5476464033126831f, 0.8856732845306396f, 
    -0.5850160717964172f, -0.9409302473068237f, 
    1.0f, 0.6183391213417053f, 1.0f, 
    -0.615153431892395f, -0.9412328004837036f, 
    1.0f, 0.6183391213417053f, 1.0f, 
    -0.5631667971611023f, -0.9562366008758545f, 
    1.0f, 0.6183391213417053f, 1.0f, 
    -0.6460562348365784f, -0.9568508863449097f, 
    1.0f, 0.6183391213417053f, 1.0f, 
    -0.5554963946342468f, -0.9837208390235901f, 
    1.0f, 0.6183391213417053f, 1.0f, 
    -0.6700929999351501f, -0.9840363264083862f, 
};
// clang-format on

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
