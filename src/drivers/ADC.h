/**
 * @addtogroup  adc
 * @{
 *
 * @file
 * @author      Bryan McElvy
 * @brief       Driver module for analog-to-digital conversion (ADC).
 */

#ifndef ADC_H
#define ADC_H

#include "GPIO.h"

#include "tm4c123gh6pm.h"

#include <stdint.h>

/// @brief   Initialize ADC0 as a single-input analog-to-digital converter.
void ADC_Init(void);

/// @brief   Enable the ADC interrupt.
void ADC_InterruptEnable(void);

/// @brief   Disable the ADC interrupt.
void ADC_InterruptDisable(void);

/// @brief   Acknowledge the ADC interrupt, clearing the flag.
void ADC_InterruptAcknowledge(void);

#endif               // ADC_H

/** @} */
