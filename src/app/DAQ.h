/**
 * @addtogroup daq
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Application software for handling data acquision (DAQ) functions.
 */

#ifndef __DAQ_H__
#define __DAQ_H__

#include "ADC.h"
#include "Timer.h"
#include "arm_math_types.h"
#include "dsp/filtering_functions.h"

#include "FIFO.h"

#include "tm4c123gh6pm.h"
#include <stdint.h>

void DAQ_Init(void);

float32_t DAQ_Filter(volatile float32_t inputSample);

#endif

/** @} */
