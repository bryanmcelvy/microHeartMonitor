/**
 * @addtogroup  timer
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for Timer module.
 */

/******************************************************************************
Preprocessor Directives
*******************************************************************************/

#include "TimerNew.h"

#include "ISR.h"
#include "NewAssert.h"

#include "tm4c123gh6pm.h"

#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
Declarations
*******************************************************************************/

enum {
    TIMER0_BASE = 0x40030000,
    TIMER1_BASE = 0x40031000,
    TIMER2_BASE = 0x40032000,
    TIMER3_BASE = 0x40033000,
    TIMER4_BASE = 0x40034000,
    TIMER5_BASE = 0x40035000
};

enum {
    CONFIG_R_OFFSET = 0x00,
    MODE_R_OFFSET = 0x04,
    CTRL_R_OFFSET = 0x0C,
    INTERVAL_LOAD_R_OFFSET = 0x28,
    VALUE_R_OFFSET = 0x054
};

typedef volatile uint32_t * register_t;

typedef struct TimerStruct_t {
    const uint32_t BASE_ADDRESS;
    register_t controlRegister;
    register_t intervalLoadRegister;
    bool isInit;
} TimerStruct_t;

// clang-format off
static TimerStruct_t TIMER_POOL[6] = {
    { TIMER0_BASE, (register_t) TIMER0_BASE + CTRL_R_OFFSET, (register_t) TIMER0_BASE + INTERVAL_LOAD_R_OFFSET, false },
    { TIMER1_BASE, (register_t) TIMER1_BASE + CTRL_R_OFFSET, (register_t) TIMER1_BASE + INTERVAL_LOAD_R_OFFSET, false },
    { TIMER2_BASE, (register_t) TIMER2_BASE + CTRL_R_OFFSET, (register_t) TIMER2_BASE + INTERVAL_LOAD_R_OFFSET, false },
    { TIMER3_BASE, (register_t) TIMER3_BASE + CTRL_R_OFFSET, (register_t) TIMER3_BASE + INTERVAL_LOAD_R_OFFSET, false },
    { TIMER4_BASE, (register_t) TIMER4_BASE + CTRL_R_OFFSET, (register_t) TIMER4_BASE + INTERVAL_LOAD_R_OFFSET, false },
    { TIMER5_BASE, (register_t) TIMER5_BASE + CTRL_R_OFFSET, (register_t) TIMER5_BASE + INTERVAL_LOAD_R_OFFSET, false }};

// clang-format on

/******************************************************************************
Initialization
*******************************************************************************/

Timer_t Timer_Init(timerName_t timerName, bool isPeriodic, bool isCountingUp) {
    Timer_t timer = &TIMER_POOL[timerName];
    if(timer->isInit == false) {
        // Start clock to timer
        SYSCTL_RCGCTIMER_R |= (1 << timerName);
        while((SYSCTL_PRTIMER_R & (1 << timerName)) == 0) {}
        timer->isInit = true;
    }

    // Disable timers and turn on concatenated mode
    *timer->controlRegister &= ~(0x0101);
    *((register_t) (timer->BASE_ADDRESS + CONFIG_R_OFFSET)) &= ~(0x0007);

    // Configure timer according to input args
    *((register_t) (timer->BASE_ADDRESS + MODE_R_OFFSET)) &= ~(0x0FFF);

    if(isCountingUp) {
        *((register_t) (timer->BASE_ADDRESS + MODE_R_OFFSET)) |= 0x10;
    }
    else {
        *((register_t) (timer->BASE_ADDRESS + MODE_R_OFFSET)) &= ~(0x10);
    }

    if(isPeriodic) {
        *((register_t) (timer->BASE_ADDRESS + MODE_R_OFFSET)) |= 0x02;
    }
    else {
        *((register_t) (timer->BASE_ADDRESS + MODE_R_OFFSET)) |= 0x01;
    }

    return timer;
}

/******************************************************************************
Configuration
*******************************************************************************/

void Timer_enableAdcTrigger(Timer_t timer) {
    Assert(timer->isInit);

    *timer->controlRegister |= 0x20;
    return;
}

void Timer_disableAdcTrigger(Timer_t timer) {
    Assert(timer->isInit);

    *timer->controlRegister &= ~(0x20);
    return;
}

void Timer_setInterval_ms(Timer_t timer, uint32_t time_ms) {
    Assert(timer->isInit);
    Assert((time_ms > 0) && (time_ms <= 53000));

    *timer->controlRegister &= ~(0x101);               // disable timer
    uint32_t reload_val = (80000 * time_ms) - 1;
    *timer->intervalLoadRegister = reload_val;

    return;
}

/******************************************************************************
Basic Operations
*******************************************************************************/

uint32_t Timer_getCurrentValue(Timer_t timer) {
    Assert(timer->isInit);

    return *((register_t) timer->BASE_ADDRESS + VALUE_R_OFFSET);
}

void Timer_Start(Timer_t timer) {
    Assert(timer->isInit);

    *timer->controlRegister |= 0x101;                  // enable timer
    return;
}

void Timer_Stop(Timer_t timer) {
    Assert(timer->isInit);

    *timer->controlRegister &= ~(0x101);               // stop/disable timer
    return;
}

bool Timer_isCounting(Timer_t timer) {
    Assert(timer->isInit);

    return *timer->controlRegister & 0x101;
}

void Timer_Wait1ms(Timer_t timer, uint32_t time_ms) {
    Assert(timer->isInit);

    Timer_setInterval_ms(timer, time_ms);
    Timer_Start(timer);
    while(Timer_isCounting(timer)) {}

    return;
}

/** @} */
