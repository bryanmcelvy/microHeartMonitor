/**
 * @addtogroup qrs
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   QRS detection algorithm functions.
 *
 *          This module contains functions for detecting heart rate (`HR`) using
 *          a simplified version of the Pan-Tompkins algorithm.
 */

/*******************************************************************************
Preprocessor Directives
********************************************************************************/

#ifndef QRS_H
#define QRS_H

#include "arm_math_types.h"
#include "dsp/filtering_functions.h"
#include "dsp/statistics_functions.h"

#include <stdbool.h>
#include <stdint.h>

#define QRS_SAMP_FREQ 200                     // [Hz]
#define QRS_NUM_SAMP  (1 << 10)               // number of samples to process after calibration

/*******************************************************************************
Functions
********************************************************************************/

///@brief           Initialize the QRS detector.
void QRS_Init(void);

/**
 * @brief                   Preprocess the raw ECG data.
 *
 * @param[in] inputBuffer   Array of raw ECG signal values.
 * @param[in] outputBuffer  Array of preprocessed ECG signal values.
 */
void QRS_Preprocess(float32_t inputBuffer[], float32_t outputBuffer[]);

/**
 * @brief                   Apply decision rules to the data.
 *
 * @param[in] inputBuffer   Array of preprocessed ECG signal values.
 * @param[out] float32_t    Average heart rate in [bpm].
 */
float32_t QRS_ApplyDecisionRules(float32_t inputBuffer[]);

/**
 * @brief                   Run the full algorithm on the inputted ECG data.
 *
 * @param[in] inputBuffer   Array of raw ECG signal values.
 * @param[in] outputBuffer  Array of preprocessed ECG signal values.
 * @param[out] float32_t    Average heart rate in [bpm].
 */
float32_t QRS_RunDetection(float32_t inputBuffer[], float32_t outputBuffer[]);

#endif               // QRS_H

/** @} */
