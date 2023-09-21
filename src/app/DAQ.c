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

#include "arm_math_types.h"
#include "dsp/filtering_functions.h"
#include "tm4c123gh6pm.h"

#include <stdint.h>

#define SAMPLING_PERIOD_MS 5               ///< sampling period in ms (\f$ T_s = 1 / f_s \f$)

/******************************************************************************
Input Filter Definition
*******************************************************************************/
enum { NUM_STAGES = 6, NUM_COEFFICIENTS = NUM_STAGES * 5, STATE_BUFF_SIZE = NUM_STAGES * 4 };

typedef arm_biquad_casd_df1_inst_f32 filt_t;

static filt_t inputFilter = { 0 };
static float32_t stateBuffer_Input[STATE_BUFF_SIZE];

// NOLINTBEGIN(*-narrowing-conversions)
// clang-format off

// static const float32_t COEFF_INPUT[10] = { 
//     0.046582907, 0.093165815, 0.04658291, -0.32897568, 0.06458765,
//     1.0, 2.0, 0.99999994, -0.45311952, 0.4663256
// };

static const float32_t DAQ_FILT_COEFF[NUM_COEFFICIENTS] = {
    0.04290771484375, 0.0858154296875, 0.042938232421875, -0.328857421875, 0.0645751953125, 
    1.0, 1.9990234375, 0.99951171875, -0.453125, 0.46630859375, 
    1.0, 0.6181640625, 1.0, 0.57861328125, 0.943359375, 
    1.0, 0.61865234375, 1.0009765625, 0.623046875, 0.94384765625, 
    1.0, 0.6181640625, 0.99951171875, 0.5556640625, 0.97607421875, 
    1.0, 0.61865234375, 0.99951171875, 0.6650390625, 0.9765625
};
// NOLINTEND(*-narrowing-conversions)
// clang-format on

/******************************************************************************
Functions
*******************************************************************************/
void DAQ_Init(void) {
    arm_biquad_cascade_df1_init_f32((filt_t *) &inputFilter, NUM_STAGES, DAQ_FILT_COEFF,
                                    stateBuffer_Input);
    ADC_Init();
    Timer3A_Init(SAMPLING_PERIOD_MS);

    return;
}

float32_t DAQ_Filter(volatile float32_t inputSample) {
    volatile float32_t outputSample;

    arm_biquad_cascade_df1_f32((const filt_t *) &inputFilter, (const float32_t *) &inputSample,
                               (float32_t *) &outputSample, 1);

    return outputSample;
}

/** @} */
