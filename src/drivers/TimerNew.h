/**
 * @addtogroup  timer
 * @{
 *
 * @file
 * @author      Bryan McElvy
 * @brief       Driver module for general-purpose timer modules.

                Timer | Function
                ---------------------
                0A      Debouncing
                1A      LCD Interrupts
                2A      ILI9341 Resets
                3A      ADC Interrupts
 */

#ifndef TIMER_H
#define TIMER_H

#include "tm4c123gh6pm.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct TimerStruct_t * Timer_t;

Timer_t Timer_Init(uint8_t timerNum);

void Timer_Start(Timer_t timer, uint32_t time_ms);

bool Timer_isCounting(Timer_t timer);

void Timer_Wait1ms(Timer_t timer, uint32_t time_ms);

#endif               // TIMER_H

/** @} */
