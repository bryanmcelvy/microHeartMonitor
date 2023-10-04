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

#ifndef QRS_H
#define QRS_H

/*******************************************************************************
Preprocessor Directives
********************************************************************************/

#include "arm_math_types.h"
#include "dsp/filtering_functions.h"
#include "dsp/statistics_functions.h"

#include <stdbool.h>
#include <stdint.h>

#define QRS_SAMP_FREQ 200                               // [Hz]
#define QRS_NUM_SAMP  (uint16_t)(1 << 10)               // num. samples to process

/*******************************************************************************
Functions
********************************************************************************/

///@brief           Initialize the QRS detector.
void QRS_Init(void);

/**
 * @brief                   Preprocess the ECG data to remove noise and/or
 *                          exaggerate the signal characteristic(s) of interest.
 *
 * @pre                     Fill `inputBuffer` with raw or lightly preprocessed
 *                          ECG data.
 *
 * @param[in] xn            Array of raw ECG signal values.
 * @param[in] yn            Array used to hold preprocessed ECG signal values.
 *
 * @post                    `yn` will contain the preprocessed data,
 *                          which is ready to be analyzed to calculate HR.
 *
 * @see                     QRS_applyDecisionRules()
 */
void QRS_Preprocess(const float32_t xn[], float32_t yn[]);

/**
 * @brief                   Calculate the average heart rate (HR) using
 *                          predetermined decision rules.
 *
 * @pre                     Preprocess the raw ECG data.
 *
 * @param[in] yn            Array of preprocessed ECG signal values.
 * @param[out] float32_t    Average heart rate in [bpm].
 *
 * @post                    Certain information (signal/noise levels, thresholds, etc.)
 *                          is retained between calls.
 *
 * @see                     QRS_Preprocess()
 */
float32_t QRS_applyDecisionRules(const float32_t yn[]);

/**
 * @brief                   Run the full algorithm (preprocessing and decision rules)
 *                          on the inputted ECG data.
 *
 *                          This function simply combines the preprocessing and
 *                          decision rules functions into a single function.
 *
 * @param[in] xn            Array of raw ECG signal values.
 * @param[in] yn            Array used to hold preprocessed ECG signal values.
 * @param[out] float32_t    Average heart rate in [bpm].
 *
 * @post                    `yn` will contain the preprocessed data.
 * @post                    Certain information (signal/noise levels, thresholds, etc.)
 *                          is retained between calls.
 *
 * @see                     QRS_Preprocess(), QRS_applyDecisionRules()
 */
float32_t QRS_runDetection(const float32_t xn[], float32_t yn[]);

#endif               // QRS_H

/** @} */
