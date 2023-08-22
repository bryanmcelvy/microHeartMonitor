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
#include "arm_math_types.h"
#include "dsp/filtering_functions.h"

#include "FIFO.h"

#include "tm4c123gh6pm.h"
#include <stdint.h>

#define SAMPLING_PERIOD_MS 5               ///< sampling period in ms (\f$ T_s = 1 / f_s \f$)

/******************************************************************************
Input Filter Definition
*******************************************************************************/
typedef arm_biquad_casd_df1_inst_f32 filt_t;
static filt_t inputFilter = { 0 };
static const float32_t COEFF_INPUT[10] = { (float32_t) 0.046582907, (float32_t) 0.093165815,
                                           (float32_t) 0.04658291,  (float32_t) -0.32897568,
                                           (float32_t) 0.06458765,  (float32_t) 1.0,
                                           (float32_t) 2.0,         (float32_t) 0.99999994,
                                           (float32_t) -0.45311952, (float32_t) 0.4663256 };
static float32_t stateBuffer_Input[8];

/******************************************************************************
Functions
*******************************************************************************/
void DAQ_Init(void) {
    arm_biquad_cascade_df1_init_f32((arm_biquad_casd_df1_inst_f32 *) &inputFilter, 2, COEFF_INPUT,
                                    stateBuffer_Input);
    ADC_Init();
    Timer3A_Init(SAMPLING_PERIOD_MS);
}

volatile float32_t DAQ_Filter(volatile float32_t inputSample) {
    volatile float32_t outputSample;

    arm_biquad_cascade_df1_f32((const arm_biquad_casd_df1_inst_f32 *) &inputFilter,
                               (const float32_t *) &inputSample, (float32_t *) &outputSample, 1);

    return outputSample;
}

/** @} */
