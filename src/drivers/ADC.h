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

// #include "FIFO.h"
// #include "lookup.h"
#include "Timer.h"

#include "tm4c123gh6pm.h"
#include <stdint.h>

/**
 * @brief   Initialize ADC0 as a differential-input analog-to-digital converter.
 *
 */
void ADC_Init(void);

// /**
//  * @brief           Convert a raw ADC sample to voltage in [mV].
//  *
//  * @param sample    12-bit unsigned ADC value. `sample = [0, 0xFFF]`
//  * @return double    Voltage value in range `[-3.3, 3.3) [mV]`.
//  */
// double ADC_ConvertToVolts(uint16_t raw_sample);

#endif

/** @} */
