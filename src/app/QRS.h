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

#ifndef DETECTOR_H
#define DETECTOR_H

#include "FIFO.h"

#include "arm_math_types.h"
#include "dsp/filtering_functions.h"

#define DETECTOR_NUM_SAMPLES 1000               // number of samples to process after calibration

/*******************************************************************************
Functions
********************************************************************************/

/// @brief Initialize the QRS detector.
void Detector_Init(void);

/**
 * @brief                       Calculate the heart rate from the ECG data.
 *
 * @param[in] inputDataFifo     FIFO buffer containing ECG data.
 * @param[out] float32_t        Average heart rate in [bpm].
 */
float32_t Detector_RunDetection(FIFO_t * inputDataFifo);

#endif               // DETECTOR_H

/** @} */
