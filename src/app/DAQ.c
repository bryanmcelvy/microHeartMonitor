/**
 * @addtogroup daq
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for DAQ module.
 */

#include "DAQ.h"

/*******************************************************************************
SECTIONS
        Preprocessor Directives
        Digital Filter Variables
        Initialization
        Reading Input Data
        Digital Filtering Functions
********************************************************************************/

/******************************************************************************
Preprocessor Directives
*******************************************************************************/

#include "ADC.h"
#include "Timer.h"

#include "NewAssert.h"

#include "arm_math_types.h"
#include "dsp/filtering_functions.h"
#include "tm4c123gh6pm.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#define SAMPLING_PERIOD_MS 5               ///< sampling period in ms (\f$ T_s = \frac{1}{f_s} \f$)

/******************************************************************************
Digital Filter Variables
*******************************************************************************/
/** @name Digital Filters */               /// @{

enum {
    NUM_STAGES_NOTCH = 6,
    NUM_COEFFS_NOTCH = NUM_STAGES_NOTCH * 5,
    STATE_BUFF_SIZE_NOTCH = NUM_STAGES_NOTCH * 4,

    NUM_STAGES_BANDPASS = 4,
    NUM_COEFFS_DAQ_BANDPASS = NUM_STAGES_BANDPASS * 5,
    STATE_BUFF_SIZE_BANDPASS = NUM_STAGES_BANDPASS * 4
};

/* clang-format off */
static const float32_t COEFFS_NOTCH[NUM_COEFFS_NOTCH] = { 
    // Section 1
    0.8856732845306396f, 0.5476464033126831f, 0.8856732845306396f, 
    -0.5850160717964172f, -0.9409302473068237f, 
    // Section 2
    1.0f, 0.6183391213417053f, 1.0f, 
    -0.615153431892395f, -0.9412328004837036f, 
    // Section 3
    1.0f, 0.6183391213417053f, 1.0f, 
    -0.5631667971611023f, -0.9562366008758545f, 
    // Section 4
    1.0f, 0.6183391213417053f, 1.0f, 
    -0.6460562348365784f, -0.9568508863449097f, 
    // Section 5
    1.0f, 0.6183391213417053f, 1.0f, 
    -0.5554963946342468f, -0.9837208390235901f, 
    // Section 6
    1.0f, 0.6183391213417053f, 1.0f, 
    -0.6700929999351501f, -0.9840363264083862f, 
};

static const float32_t COEFFS_BANDPASS[NUM_COEFFS_DAQ_BANDPASS] = {
    // Section 1
    0.3240305185317993f, 0.3665695786476135f, 0.3240305185317993f, 
    -0.20968256890773773f, -0.1729172021150589f, 
    // Section 2
    1.0f, -0.4715292155742645f, 1.0f, 
    0.5868059992790222f, -0.7193671464920044f, 
    // Section 3
    1.0f, -1.9999638795852661f, 1.0f, 
    1.9863483905792236f, -0.986438512802124f, 
    // Section 4
    1.0f, -1.9997893571853638f, 1.0f, 
    1.994096040725708f, -0.9943605065345764f, 
};                                         /* clang-format on */

typedef arm_biquad_casd_df1_inst_f32 Filter_t;

static float32_t stateBuffer_Notch[STATE_BUFF_SIZE_NOTCH];
static const Filter_t notchFiltStruct = { NUM_STAGES_NOTCH, stateBuffer_Notch, COEFFS_NOTCH };
static const Filter_t * const notchFilter = &notchFiltStruct;

static float32_t stateBuffer_Bandpass[STATE_BUFF_SIZE_BANDPASS];
static const Filter_t bandpassFiltStruct = { NUM_STAGES_BANDPASS, stateBuffer_Bandpass,
                                             COEFFS_BANDPASS };
static const Filter_t * const bandpassFilter = &bandpassFiltStruct;

/** @} */                                  // Digital Filters

/*******************************************************************************
Initialization
********************************************************************************/

void DAQ_Init(void) {
    ADC_Init();

    Timer_t DAQ_Timer = Timer_Init(TIMER3);
    Timer_setMode(DAQ_Timer, PERIODIC, UP);
    Timer_enableAdcTrigger(DAQ_Timer);
    Timer_setInterval_ms(DAQ_Timer, SAMPLING_PERIOD_MS);
    Timer_Start(DAQ_Timer);

    return;
}

/*******************************************************************************
Reading Input Data
********************************************************************************/

uint16_t DAQ_readSample(void) {
    return (uint16_t) (ADC0_SSFIFO3_R & 0xFFF);
}

void DAQ_acknowledgeInterrupt(void) {
    ADC0_ISC_R |= 0x08;
    return;
}

/*******************************************************************************
Digital Filtering Functions
********************************************************************************/

float32_t DAQ_NotchFilter(volatile float32_t inputSample) {
    float32_t outputSample = 0;

    arm_biquad_cascade_df1_f32(notchFilter, (const float32_t *) &inputSample, &outputSample, 1);
    Assert(isinf(outputSample) == false);
    Assert(isnan(outputSample) == false);

    return outputSample;
}

float32_t DAQ_BandpassFilter(volatile float32_t inputSample) {
    float32_t outputSample = 0;

    arm_biquad_cascade_df1_f32(bandpassFilter, (const float32_t *) &inputSample, &outputSample, 1);
    Assert(isinf(outputSample) == false);
    Assert(isnan(outputSample) == false);

    return outputSample;
}

/** @} */
