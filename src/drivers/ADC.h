/**
 * @addtogroup  adc
 * @{
 *
 * @file
 * @author      Bryan McElvy
 * @brief       Header file for analog-to-digital conversion (ADC) module.
 */

#ifndef ADC_H
#define ADC_H

/**
 * @brief           Initialize ADC0 as a single-input analog-to-digital converter.
 * @post            Analog input 8 (`Ain8`) – AKA GPIO pin PE5 – captures samples when
 *                  triggered by one of the hardware timers, and initiates an interrupt
 *                  once sample capture is complete.
 */
void ADC_Init(void);

#endif               // ADC_H

/** @} */
