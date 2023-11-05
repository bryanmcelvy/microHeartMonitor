/**
 * @addtogroup qrs
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Header file for QRS detection module.
 */

#ifndef QRS_H
#define QRS_H

#include "arm_math_types.h"
#include <stdbool.h>

#define QRS_SAMP_FREQ       ((uint32_t) 200)                     // [Hz]
#define QRS_SAMP_PERIOD_SEC ((float32_t) 0.005f)
#define QRS_NUM_SAMP        ((uint16_t) (1 << 11))               // num. samples to process

/**
 * @brief                   Initialize the QRS detector.
 * @note                    This function isn't necessary anymore, but I'm keeping it here
 *                          just in case.
 */
void QRS_Init(void);

/**
 * @brief                   Preprocess the ECG data to remove noise and/or
 *                          exaggerate the signal characteristic(s) of interest.
 *
 * @pre                     Fill input buffer `xn` with raw or lightly preprocessed ECG data.
 *
 * @param[in] xn            Array of raw ECG signal values.
 * @param[in] yn            Array used to store preprocessed ECG signal values.
 *
 * @post                    The preprocessed signal data \f$y[n]\f$ is stored in `yn` and is ready
 *                          to be analyzed to calculate the heart rate in [bpm].
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
 * @param[out] heartRate    Average heart rate in [bpm].
 *
 * @post                    Certain information (signal/noise levels, thresholds, etc.) is retained
 *                          between calls and used to improve further detection.
 *
 * @bug                     The current implementation processes one block of data at a time and
 *                          discards the entire block immediately after. As a result, QRS complexes
 *                          that are cutoff between one block and another are not being counted.
 *
 * @see                     QRS_Preprocess()
 */
float32_t QRS_applyDecisionRules(const float32_t yn[]);

#endif               // QRS_H

/** @} */
