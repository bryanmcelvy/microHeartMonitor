/**
 * @addtogroup daq
 * @{
 *
 * @file
 * @brief   Lookup table for DAQ module.
 * @author  Bryan McElvy
 */

#ifndef LOOKUP_H
#define LOOKUP_H

#include "arm_math_types.h"

#define LOOKUP_DAQ_MAX (float32_t) 5.5
#define LOOKUP_DAQ_MIN (float32_t)(-5.5)

/**
 * @brief               Return a pointer to the DAQ lookup table.
 *
 * @param[out] lutPtr   Pointer to the lookup table (LUT).
 */
const float32_t * Lookup_GetPtr(void);

#endif               // LOOKUP_H

/** @} */
