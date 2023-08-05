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

#ifndef __TIMER_H___
#define __TIMER_H___

#include "tm4c123gh6pm.h"

#include <stdint.h>

/**********************************************************************
Timer0A
***********************************************************************/

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

/**********************************************************************
Timer1A
***********************************************************************/

/**
 * @brief   Initialize timer 1 as a 32-bit, periodic, countdown timer with interrupts.
 * 
 * @param   time_ms Time in [ms] between interrupts.
 *                  Must be <= 53 seconds.
 */
void Timer1A_Init(uint32_t time_ms);

/**********************************************************************
Timer2A
***********************************************************************/

/**
 * @brief   Initialize timer 2 as 32-bit, one-shot, countdown timer.
 * 
 */
void Timer2A_Init(void);

/**
 * @brief   Count down starting from the inputted value.
 * 
 * @param   time_ms Time in [ms] to load into Timer 2. 
 *                  Must be <= 53 seconds.
 */
void Timer2A_Start(uint32_t time_ms);

/**
 * @brief   Returns 1 if Timer2 is still counting and 0 if not.
 * 
 * @return  uint8_t status
 */
uint8_t Timer2A_isCounting(void);

/**
 * @brief   Wait for the specified amount of time in [ms].
 * 
 * @param   time_ms Time in [ms] to load into Timer 2. 
 *                  Must be <= 53 seconds.
 */
void Timer2A_Wait1ms(uint32_t time_ms);

/**********************************************************************
Timer3A
***********************************************************************/

/**
 * @brief   Initialize Timer3A as a 32-bit, periodic, countdown timer 
 *          that triggers ADC sample capture.
 * 
 * @param   time_ms     Time in [ms] to load into Timer3A. 
 *                      Must be <= 53 seconds.
 */
void Timer3A_Init(uint32_t time_ms);

#endif

/** @} */
