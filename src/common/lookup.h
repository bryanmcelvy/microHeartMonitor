/**
 * @file
 * @brief   Lookup table API.
 * @author  Bryan McElvy
 */

#ifndef LOOKUP_H
#define LOOKUP_H

#include "arm_math_types.h"

#define LOOKUP_ADC_MAX (float32_t) 5.5

/**
 * @brief   Return a pointer to the ADC lookup table.
 *
 * @return  const float32_t*
 */
const float32_t * Lookup_GetPtr_ADC(void);

#endif               // LOOKUP_H
