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

#include "arm_math_types.h"

#include "tm4c123gh6pm.h"

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
Initialization
********************************************************************************/
/** @name Initialization */               /// @{

/**
 * @brief                  Initialize the data acquisition (DAQ) module.
 *
 * @post                   The ADC and Timer are initialized, and the DAQ module
 *                         has access to its lookup table (LUT).
 */
void DAQ_Init(void);

/// @} Initialization

/*******************************************************************************
Reading Input Data
********************************************************************************/
/** @name Reading Input Data */               /// @{

/**
 * @brief                   Read a sample from the ADC.
 *
 * @pre                     Initialize the DAQ module.
 * @pre                     This should be used in an interrupt handler and/or at a
 *                          consistent rate (i.e. the sampling frequency).
 *
 * @param[out] sample       12-bit sample in range `[0x000, 0xFFF]`
 *
 * @post                    The sample can now be converted to millivolts.
 *
 * @see                     DAQ_convertToMilliVolts()
 */
uint16_t DAQ_readSample(void);

/**
 * @brief                   Convert a 12-bit ADC sample to a floating-point
 *                          voltage value via LUT.
 *
 * @pre                     Read a sample from the ADC.
 *
 * @param[in] sample        12-bit sample in range `[0x000, 0xFFF]`
 * @param[out] xn           Voltage value in range \f$ [-5.5, 5.5) [mV] \f$
 *
 * @post                    The sample \f$ x[n] \f$ is ready for filtering.
 *
 * @see                     DAQ_readSample()
 */
float32_t DAQ_convertToMilliVolts(uint16_t sample);

/// @} Reading Input Data

/*******************************************************************************
Digital Filtering Functions
********************************************************************************/
/** @name Digital Filtering Functions */               /// @{

/**
 * @brief                   Apply a 60 [Hz] notch filter to an input sample.
 *
 * @pre                     Read a sample from the ADC and convert it to millivolts.
 *
 * @param[in] xn            Raw input sample
 * @param[out] yn           Filtered output sample
 *
 * @post                    \f$ y[n] \f$ is ready for analysis and/or further processing.
 *
 * @see                     DAQ_BandpassFilter()
 */
float32_t DAQ_NotchFilter(volatile float32_t xn);

/**
 * @brief                   Apply a 0.5-40 [Hz] bandpass filter to an input sample.
 *
 * @pre                     Read a sample from the ADC and convert it to millivolts.
 *
 * @param[in] xn            Input sample
 * @param[out] yn           Filtered output sample
 *
 * @post                    \f$ y[n] \f$ is ready for analysis and/or further processing.
 *
 * @see                     DAQ_NotchFilter()
 */
float32_t DAQ_BandpassFilter(volatile float32_t xn);

/// @} Digital Filtering Functions

#endif               // DAQ_H

/** @} */
