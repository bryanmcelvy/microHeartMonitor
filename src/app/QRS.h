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
SECTIONS
    Preprocessor Directives
    Composite Functions (Declarations)
    Initialization
    Preprocessing
    Decision Rules
    Composite Function (Definitions)
********************************************************************************/

/*******************************************************************************
Preprocessor Directives
********************************************************************************/

#ifndef QRS_H
#define QRS_H

#include "FIFO.h"

#include "arm_math_types.h"
#include "dsp/filtering_functions_f16.h"

#include <stddef.h>
#include <string.h>

#define QRS_NUM_SAMPLES 1000

/*******************************************************************************
Composite Functions (Declarations)
********************************************************************************/

inline void QRS_Preprocess(const FIFO_t * input_fifo, FIFO_t * output_fifo);

inline void QRS_ApplyThresholding(const FIFO_t * input_fifo, FIFO_t * output_fifo);

inline void QRS_DetectHeartRate(const FIFO_t * input_fifo, FIFO_t * output_fifo);

/*******************************************************************************
Initialization
********************************************************************************/

void QRS_Init(void);

/*******************************************************************************
Pre-Processing
********************************************************************************/

void QRS_HighPassFilt(const FIFO_t * input_fifo, FIFO_t * output_fifo);

void QRS_LowPassFilt(const FIFO_t * input_fifo, FIFO_t * output_fifo);

void QRS_DerivativeFilt(const FIFO_t * input_fifo, FIFO_t * output_fifo);

void QRS_SquareAndIntegrate(const FIFO_t * input_fifo, FIFO_t * output_fifo);

/*******************************************************************************
Decision Rules
********************************************************************************/

/*******************************************************************************
Composite Functions (Definitions)
********************************************************************************/

inline void QRS_Preprocess(const FIFO_t * input_fifo, FIFO_t * output_fifo) {
    /// TODO: Implement
}

inline void QRS_ApplyThresholding(const FIFO_t * input_fifo, FIFO_t * output_fifo) {
    /// TODO: Implement
}

inline void QRS_DetectHeartRate(const FIFO_t * input_fifo, FIFO_t * output_fifo) {
    /// TODO: Implement
}

#endif               // QRS_H

/** @} */
