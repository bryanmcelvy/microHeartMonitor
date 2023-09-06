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

#include "lookup.h"
#include "Timer.h"

#include "arm_math_types.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>

/// @brief   Initialize ADC0 as a single-input analog-to-digital converter.
void ADC_Init(void);

/// @brief   Enable the ADC interrupt.
void ADC_InterruptEnable(void);

/// @brief   Disable the ADC interrupt.
void ADC_InterruptDisable(void);

/**
 * @brief               Convert a raw ADC sample to voltage in [mV].
 *
 * @param raw_sample    12-bit unsigned ADC value. `sample = [0, 0xFFF]`
 * @return double       Voltage value in range `[-5.5, 5.5) [mV]`.
 */
volatile float32_t ADC_ConvertToVolts(uint16_t raw_sample);

#endif

/** @} */
