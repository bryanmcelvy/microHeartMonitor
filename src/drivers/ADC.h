/**
 * @addtogroup  adc
 * @{
 *
 * @file
 * @author      Bryan McElvy
 * @brief       Driver module for analog-to-digital conversion (ADC).
 */

#ifndef __ADC_H___
#define __ADC_H___

#include "FIFO.h"
#include "Timer.h"

#include "tm4c123gh6pm.h"
#include <stdint.h>

/**
 * @brief   Initialize ADC0 as a differential-input analog-to-digital converter.
 *
 */
void ADC_Init(void);

#endif

/** @} */
