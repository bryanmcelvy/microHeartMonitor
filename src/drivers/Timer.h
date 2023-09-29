/**
 * @addtogroup  timer
 * @{
 *
 * @file
 * @author      Bryan McElvy
 * @brief       Device driver for general-purpose timer modules.
 */

#ifndef TIMER_H
#define TIMER_H

/******************************************************************************
Preprocessor Directives
*******************************************************************************/

#include "ISR.h"
#include "NewAssert.h"

#include "tm4c123gh6pm.h"

#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
Initialization
*******************************************************************************/

typedef struct TimerStruct_t * Timer_t;

typedef enum {
    TIMER0,
    TIMER1,
    TIMER2,
    TIMER3,
    TIMER4,
    TIMER5,
} timerName_t;

// TODO: Write description
Timer_t Timer_Init(timerName_t timerName);

// TODO: Write description
timerName_t Timer_getName(Timer_t timer);

/******************************************************************************
Configuration
*******************************************************************************/

// TODO: Implement more modes
typedef enum { ONESHOT, PERIODIC } timerMode_t;

enum { UP = true, DOWN = false };

// TODO: Write description
void Timer_setMode(Timer_t timer, timerMode_t timerMode, bool isCountingUp);

// TODO: Write description
void Timer_enableAdcTrigger(Timer_t timer);

// TODO: Write description
void Timer_disableAdcTrigger(Timer_t timer);

// TODO: Write description
void Timer_enableInterruptOnTimeout(Timer_t timer, uint8_t priority);

// TODO: Write description
void Timer_disableInterruptOnTimeout(Timer_t timer);

// TODO: Write description
void Timer_clearInterruptFlag(Timer_t timer);

/******************************************************************************
Basic Operations
*******************************************************************************/

// TODO: Write description
void Timer_setInterval_ms(Timer_t timer, uint32_t time_ms);

// TODO: Write description
uint32_t Timer_getCurrentValue(Timer_t timer);

// TODO: Write description
void Timer_Start(Timer_t timer);

// TODO: Write description
void Timer_Stop(Timer_t timer);

// TODO: Write description
bool Timer_isCounting(Timer_t timer);

// TODO: Write description
void Timer_Wait1ms(Timer_t timer, uint32_t time_ms);

#endif               // TIMER_H

/** @} */
