/**
 * @file
 * @brief   Lookup table API.
 * @author  Bryan McElvy
 */

#ifndef __LOOKUP_H__
#define __LOOKUP_H__

#include "arm_math_types.h"

/**
 * @brief   Return a pointer to the ADC lookup table.
 *
 * @return  const float32_t*
 */
const float32_t * Lookup_GetPtr_ADC(void);

#endif
