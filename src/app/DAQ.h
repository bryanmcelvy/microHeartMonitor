/**
 * @addtogroup daq
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Application software for handling data acquision (DAQ) functions.
 */

#ifndef DAQ_H
#define DAQ_H

/*******************************************************************************
SECTIONS
        Preprocessor Directives
        Initialization
        Reading Input Data
        Digital Filtering Functions
********************************************************************************/

/*******************************************************************************
Preprocessor Directives
********************************************************************************/

#include "lookup.h"

#include "ADC.h"
#include "Timer.h"

#include "NewAssert.h"

#include "arm_math_types.h"
#include "dsp/filtering_functions.h"
#include "tm4c123gh6pm.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
Initialization
********************************************************************************/

/**
 * @brief Initialize the data acquisition module.
 */
void DAQ_Init(void);

/*******************************************************************************
Reading Input Data
********************************************************************************/

/**
 * @brief                   Read a sample from the ADC.
 *
 * @param[out] uint16_t     12-bit sample in range [0x000, 0xFFF]
 */
uint16_t DAQ_readSample(void);

/**
 * @brief                   Convert a 12-bit integer sample to a floating-point
 *                          voltage value via lookup table (LUT).
 *
 * @param[in] sample        12-bit sample in range [0x000, 0xFFF]
 * @param[out] float32_t    Voltage value in range `[-5.5, 5.5) [mV]`.
 */
float32_t DAQ_convertToMilliVolts(uint16_t sample);

/*******************************************************************************
Digital Filtering Functions
********************************************************************************/

/**
 * @brief                   Apply a 60 [Hz] notch filter to an input sample.
 *
 * @param[in] inputSample   Raw input sample in range `[-5.5, 5.5) [V]`.
 * @param[out] float32_t    Filtered output sample.
 */
float32_t DAQ_NotchFilter(volatile float32_t inputSample);

/**
 * @brief                   Apply a 0.5-40 [Hz] bandpass filter to an input sample.
 *
 * @param[in] inputSample   Raw input sample in range `[-5.5, 5.5) [V]`.
 * @param[out] float32_t    Filtered output sample.
 */
float32_t DAQ_BandpassFilter(volatile float32_t inputSample);

#endif               // DAQ_H

/** @} */
