/**
 * @file
 * @author      Bryan McElvy
 * @brief       Driver module for using SysTick-based timing and/or interrupts.
 * 
 * @addtogroup  systick
 * @{
 */

#ifndef __SYSTICK_H___
#define __SYSTICK_H___

#include "tm4c123gh6pm.h"

#include <stdint.h>

/**
 * @brief   Initialize SysTick for timing purposes.
 * 
 */
void SysTick_Timer_Init(void);

/**
 * @brief   Delay for specified amount of time in [ms].
 *          Assumes f_bus = 80[MHz].
 */
void SysTick_Wait1ms(uint32_t delay_ms);

/**
 * @brief   Initialize SysTick for interrupts.
 * 
 * @param time_ms Time in [ms] between interrupts. Cannot be more than 200[ms].
 */
void SysTick_Interrupt_Init(uint32_t time_ms);

#endif

/** @} */
