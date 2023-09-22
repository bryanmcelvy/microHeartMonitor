/**
 * @addtogroup pll
 * @{
 *
 * @file
 * @author      Bryan McElvy
 * @brief       Driver module for activating the phase-locked-loop (PLL).
 */

#ifndef PLL_H
#define PLL_H

#include "tm4c123gh6pm.h"

#include <stdint.h>

/// @brief Initializes the phase-locked-loop (PLL), allowing a bus frequency of 80[MHz].
void PLL_Init(void);

#endif               // PLL_H

/** @} */
