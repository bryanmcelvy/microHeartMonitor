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

/**
 * @brief       Initialize the phase-locked-loop to change the bus frequency.
 * @post        The bus frequency is now running at 80 [MHz].
 */
void PLL_Init(void);

#endif               // PLL_H

/** @} */
