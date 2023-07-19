/**
 * @file
 * @author      Bryan McElvy
 * @brief       Driver module for timing (Timer0) and interrupts (Timer1).
 * 
 * @addtogroup  timer
 * @{
 */

#ifndef __TIMER_H___
#define __TIMER_H___

#include "tm4c123gh6pm.h"

/**
 * @brief   Initialize timer 0 as 32-bit, one-shot, countdown timer.
 * 
 */
void Timer0A_Init(void);

/**
 * @brief   Count down starting from the inputted value.
 * 
 * @param   time_ms Time in [ms] to load into Timer 0. 
 *                  Must be <= 53 seconds.
 */
void Timer0A_Start(uint32_t time_ms);

/**
 * @brief   Returns 1 if Timer0 is still counting and 0 if not.
 * 
 * @return  uint8_t status
 */
uint8_t Timer0A_isCounting(void);

/**
 * @brief   Wait for the specified amount of time in [ms].
 * 
 * @param   time_ms Time in [ms] to load into Timer 0. 
 *                  Must be <= 53 seconds.
 */
void Timer0A_Wait1ms(uint32_t time_ms);

/**
 * @brief   Initialize timer 1 as a 32-bit, periodic, countdown timer with interrupts.
 * 
 * @param   time_ms Time in [ms] between interrupts.
 *                  Must be <= 53 seconds.
 */
void Timer1A_Init(uint32_t time_ms);

#endif

/** @} */
