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

#include <stdbool.h>
#include <stdint.h>

#define DAQ_LOOKUP_MAX ((float32_t) 5.5f)                  ///< maximum lookup table value
#define DAQ_LOOKUP_MIN ((float32_t) (-5.5f))               ///< minimum lookup table value

/*******************************************************************************
Initialization
********************************************************************************/
/** @name Initialization */               /// @{

/**
 * @brief               Initialize the data acquisition (DAQ) module.
 *
 * @post                The analog-to-digital converter (ADC) is initialized and
 *                      configured for timer-triggered sample capture.
 * @post                The timer is initialized in `PERIODIC` mode and triggers the
 *                      ADC every \f$ 5 ms \f$ (i.e. sampling frequency \f$ f_s = 200 Hz \f$).
 */
void DAQ_Init(void);

/// @} Initialization

/*******************************************************************************
Reading Input Data
********************************************************************************/
/** @name Reading Input Data */               /// @{

/**
 * @brief               Read a sample from the ADC.
 *
 * @pre                 Initialize the DAQ module.
 * @pre                 This should be used in an interrupt handler and/or at a
 *                      consistent rate (i.e. the sampling frequency).
 *
 * @param[out] sample   12-bit sample in range `[0x000, 0xFFF]`
 *
 * @post                The sample can now be converted to millivolts.
 *
 * @see                 DAQ_convertToMilliVolts()
 */
uint16_t DAQ_readSample(void);

/**
 * @brief               Convert a 12-bit ADC sample to a floating-point
 *                      voltage value via LUT.
 *
 * @pre                 Read a sample from the ADC.
 *
 * @param[in] sample    12-bit sample in range `[0x000, 0xFFF]`
 * @param[out] xn       Voltage value in range \f$ [-5.5, 5.5) [mV] \f$
 *
 * @post                The sample \f$ x[n] \f$ is ready for filtering.
 *
 * @see                 DAQ_readSample()
 * @note                Defined in @ref DAQ_lookup.c rather than @ref DAQ.c.
 */
float32_t DAQ_convertToMilliVolts(uint16_t sample);

/**
 * @brief               Acknowledge the ADC interrupt.
 * @pre                 This should be used within an interrupt handler.
 */
void DAQ_acknowledgeInterrupt(void);

/// @} Reading Input Data

/*******************************************************************************
Digital Filtering Functions
********************************************************************************/
/** @name Digital Filtering Functions */               /// @{

/**
 * @brief               Apply a 60 [Hz] notch filter to an input sample.
 *
 * @pre                 Read a sample from the ADC and convert it to millivolts.
 *
 * @param[in] xn        Raw input sample
 * @param[out] yn       Filtered output sample
 *
 * @post                \f$ y[n] \f$ is ready for analysis and/or further processing.
 *
 * @see                 DAQ_BandpassFilter()
 *
 * @image latex filters/daq_notch.png "Frequency domain parameters for the notch filter."
 */
float32_t DAQ_NotchFilter(volatile float32_t xn);

/**
 * @brief               Apply a 0.5-40 [Hz] bandpass filter to an input sample.
 *
 * @pre                 Read a sample from the ADC and convert it to millivolts.
 *
 * @param[in] xn        Input sample
 * @param[out] yn       Filtered output sample
 *
 * @post                \f$ y[n] \f$ is ready for analysis and/or further processing.
 *
 * @see                 DAQ_NotchFilter()
 *
 * @image latex filters/daq_bandpass.png "Frequency domain parameters for the bandpass filter."
 */
float32_t DAQ_BandpassFilter(volatile float32_t xn);

/// @} Digital Filtering Functions

#endif               // DAQ_H

/** @} */
