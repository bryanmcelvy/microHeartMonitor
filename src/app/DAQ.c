/**
 * @addtogroup DAQ
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for DAQ module.
 */

#include "DAQ.h"

#include "ADC.h"
#include "Timer.h"

#include "FIFO.h"
#include "Filter.h"

#include "tm4c123gh6pm.h"
#include <stdint.h>

#define SAMPLING_PERIOD_MS 5                    ///< sampling period in ms ($\f T_s = 1 / f_s $\f)

void DAQ_Init(void) {
    ADC_Init();
    Timer3A_Init(SAMPLING_PERIOD_MS);
}

/** @} */
