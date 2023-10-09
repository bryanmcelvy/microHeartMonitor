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

/**
 * @brief                   Initialize a hardware timer.
 *
 * @param[in] timerName     Name of the hardware timer to use.
 * @param[out] timer        Pointer to timer object.
 *
 * @post                    The timer is ready to be configured and used.
 *
 * @see                     Timer_isInit(), Timer_Deinit()
 */
Timer_t Timer_Init(timerName_t timerName);

/**
 * @brief                   De-initialize a hardware timer.
 *
 * @param[in] timerName     Name of the hardware timer to use.
 *
 * @post                    The hardware timer is no longer initialized or receiving power.
 *
 * @see                     Timer_Init(), Timer_isInit()
 */
void Timer_Deinit(Timer_t timer);

/**
 * @brief                   Get the name of a timer object.
 *
 * @param[in] timer         Pointer to timer object.
 * @param[out] timerName_t  Name of the hardware timer being used.
 */
timerName_t Timer_getName(Timer_t timer);

/**
 * @brief                   Check if a timer object is initialized.
 *
 * @param[in] timer         Pointer to timer object.
 * @param[out] true         The timer is initialized.
 * @param[out] false        The timer is not initialized.
 *
 * @see                     Timer_Init(), Timer_Deinit()
 */
bool Timer_isInit(Timer_t timer);

/******************************************************************************
Configuration
*******************************************************************************/

// TODO: Implement more modes
typedef enum {
    ONESHOT,               ///< the timer runs once, then stops
    PERIODIC               ///< the timer runs continuously once started
} timerMode_t;

typedef enum {
    UP,                    ///< the timer starts and 0 and counts to the reload value
    DOWN                   ///< the timer starts at its reload value and counts down
} timerDirection_t;

/**
 * @brief                       Set the mode for the timer.
 *
 * @param[in] timer             Pointer to timer object.
 * @param[in] timerMode         Mode for hardware timer to use.
 * @param[in] timerDirection    Direction to count towards.
 */
void Timer_setMode(Timer_t timer, timerMode_t timerMode, timerDirection_t timerDirection);

/**
 * @brief                       Set the timer to trigger ADC sample capture once it reaches
 *                              timeout (i.e. down to 0 or up to its reload value).
 *
 * @pre                         Initialize and configure an ADC module to be timer-triggered.
 * @param[in] timer             Pointer to timer object.
 * @post                        A timeout event triggers ADC sample capture.
 *
 * @see                         Timer_disableAdcTrigger()
 */
void Timer_enableAdcTrigger(Timer_t timer);

/**
 * @brief                       Disable ADC sample capture on timeout.
 *
 * @pre                         Initialize and configure an ADC module to be timer-triggered.
 * @param[in] timer             Pointer to timer object.
 * @post                        A timeout event no longer triggers ADC sample capture.
 *
 * @see                         Timer_enableAdcTrigger()
 */
void Timer_disableAdcTrigger(Timer_t timer);

/**
 * @brief                       Set the timer to trigger an interrupt on timeout.
 *
 * @pre                         Configure the interrupt service routine using the ISR module.
 *
 * @param[in] timer             Pointer to timer object.
 *
 * @post                        Upon timeout, an interrupt is triggered.
 *
 * @see                         Timer_disableInterruptOnTimeout()
 */
void Timer_enableInterruptOnTimeout(Timer_t timer);

/**
 * @brief                       Stop the timer from triggering interrupts on timeout.
 *
 * @param[in] timer             Pointer to timer object.
 *
 * @post                        Timeout no longer triggers ADC sample capture.
 *
 * @see                         Timer_enableInterruptOnTimeout()
 */
void Timer_disableInterruptOnTimeout(Timer_t timer);

/**
 * @brief                       Clear the timer's interrupt flag to acknowledge the interrupt.
 * @pre                         Call this during a timer's interrupt service routine (ISR).
 * @param[in] timer             Pointer to timer object.
 */
void Timer_clearInterruptFlag(Timer_t timer);

/******************************************************************************
Basic Operations
*******************************************************************************/

/**
 * @brief                       Set the interval to use.
 *
 * @pre                         Initialize and configure the timer.
 *
 * @param[in] timer             Pointer to timer object.
 * @param[in] time_ms           Time in [ms].
 *
 * @post                        Upon starting, the Timer counts down from or up to this value.
 *
 * @see                         Timer_Init(), Timer_setMode()
 */
void Timer_setInterval_ms(Timer_t timer, uint32_t time_ms);

// TODO: Write description
uint32_t Timer_getCurrentValue(Timer_t timer);

/**
 * @brief                       Start the timer.
 * @pre                         Initialize and configure the timer.
 * @param[in] timer             Pointer to timer object.
 * @post                        The timer is counting.
 * @see                         Timer_Stop(), Timer_isCounting()
 */
void Timer_Start(Timer_t timer);

/**
 * @brief                       Stop the timer.
 * @pre                         Start the timer.
 * @param[in] timer             Pointer to timer object.
 * @post                        The timer is no longer counting.
 * @see                         Timer_Start(), Timer_isCounting()
 */
void Timer_Stop(Timer_t timer);

/**
 * @brief                       Check if the timer is currently counting.
 *
 * @param[in] timer             Pointer to timer object.
 * @param[out] true             The timer is counting.
 * @param[out] false            The timer is not counting.
 *
 * @see                         Timer_Start(), Timer_Stop()
 */
bool Timer_isCounting(Timer_t timer);

/**
 * @brief                       Initiate a time delay.
 *
 * @pre                         Initialize and configure the timer.
 *
 * @param[in] timer             Pointer to timer object.
 * @param[in] time_ms           Time in [ms] to wait for.
 *
 * @post                        The program is delayed for the desired time.
 */
void Timer_Wait1ms(Timer_t timer, uint32_t time_ms);

#endif               // TIMER_H

/** @} */
