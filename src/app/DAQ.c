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
    NUM_STAGES_INPUT = 2,
    NUM_COEFF_INPUT = NUM_STAGES_INPUT * 5,
    STATE_BUFF_SIZE_INPUT = NUM_STAGES_INPUT * 4,

    // Notch Filter: 1st Order 60 [Hz] Bandstop Filter
    NUM_STAGES_NOTCH = 1,
    NUM_COEFF_NOTCH = NUM_STAGES_NOTCH * 5,
    STATE_BUFF_SIZE_NOTCH = NUM_STAGES_NOTCH * 4
};

// clang-format off

static const float32_t DAQ_FILT_COEFF_INPUT[NUM_COEFF_INPUT] = { 
    (float32_t) 0.046582907, (float32_t) 0.093165815, (float32_t) 0.04658291,
    (float32_t) -0.32897568, (float32_t) 0.06458765,
    (float32_t) 1.0, (float32_t) 2.0, (float32_t) 0.99999994,
    (float32_t) -0.45311952, (float32_t) 0.4663256
};

// TODO: Fix notch filter
static const float32_t DAQ_FILT_COEFF_NOTCH[NUM_COEFF_NOTCH] = {
    0.7548828125f,
    0.490478515625f,
    0.7548828125f,
    0.490478515625f,
    0.509765625f
};
// clang-format on

static filt_t inputFilter = { 0 };
static float32_t stateBuffer_Input[STATE_BUFF_SIZE_INPUT];

static filt_t notchFilter = { 0 };
static float32_t stateBuffer_Notch[STATE_BUFF_SIZE_NOTCH];

/******************************************************************************
Functions
*******************************************************************************/
void DAQ_Init(void) {
    arm_biquad_cascade_df1_init_f32((filt_t *) &inputFilter, NUM_STAGES_INPUT, DAQ_FILT_COEFF_INPUT,
                                    stateBuffer_Input);
    // arm_biquad_cascade_df1_init_f32((filt_t *) &notchFilter, NUM_STAGES_NOTCH,
    // DAQ_FILT_COEFF_NOTCH,
    //                                 stateBuffer_Notch);
    ADC_Init();
    Timer3A_Init(SAMPLING_PERIOD_MS);

    return;
}

float32_t DAQ_Filter(volatile float32_t inputSample) {
    volatile float32_t outputSample1 = 0;
    volatile float32_t outputSample2 = 0;

    arm_biquad_cascade_df1_f32((const filt_t *) &inputFilter, (const float32_t *) &inputSample,
                               (float32_t *) &outputSample1, 1);
    // arm_biquad_cascade_df1_f32((const filt_t *) &notchFilter, (const float32_t *) &outputSample1,
    //                            (float32_t *) &outputSample2, 1);

    // Assert(isinf(outputSample2) == false);

    return outputSample1;
}

/** @} */
