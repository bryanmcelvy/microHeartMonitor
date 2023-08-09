/**
 * @addtogroup DAQ
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Application software for handling data acquision (DAQ) functions.
 */

#ifndef __DAQ_H__
#define __DAQ_H__

#include "ADC.h"
#include "Timer.h"

#include "FIFO.h"
#include "Filter.h"

#include "tm4c123gh6pm.h"
#include <stdint.h>

void DAQ_Init(void);

#endif
