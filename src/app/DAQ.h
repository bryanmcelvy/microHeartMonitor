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

#include "ADC.h"
#include "Timer.h"

#include "FIFO.h"

#include "arm_math_types.h"
#include "dsp/filtering_functions.h"
#include "tm4c123gh6pm.h"

#include <stdint.h>

/**
 * @brief Initialize the data acquisition module, including the input filter
 * and timer interrupt-based analog-to-digital conversion (ADC) @ \f$ f_s = 200 [Hz] \f$
 *
 */
void DAQ_Init(void);

/**
 * @brief                   Filter an input sample using a 40 [Hz] low pass
 *                          filter and a 60 [Hz] notch filter.
 *
 * @param[in] inputSample   Raw input sample in range `[-5.5, 5.5) [V]`.
 * @param[out] float32_t    Filtered output sample.
 */
float32_t DAQ_Filter(volatile float32_t inputSample);

#endif               // DAQ_H

/** @} */
