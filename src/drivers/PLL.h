/**
 * @file
 * @author      Bryan McElvy
 * @brief       Driver module for activating the phase-locked-loop (PLL).
 * 
 * @addtogroup  pll
 * @{
 */

#ifndef __PLL_H___
#define __PLL_H___

#include "tm4c123gh6pm.h"

/**
 * @brief Initializes the phase-locked-loop (PLL), allowing a bus frequency of 80[MHz].
 * 
 */
void PLL_Init(void);

#endif

/** @} */
