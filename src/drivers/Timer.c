/**
 * @addtogroup  timer
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for Timer module.
 */

#include "Timer.h"

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

enum REGISTER_OFFSETS {
    CONFIG = 0x00,
    MODE = 0x04,
    CTRL = 0x0C,
    INT_MASK = 0x18,
    INT_CLEAR = 0x24,
    INTERVAL = 0x28,
    VALUE = 0x054
};

typedef volatile uint32_t * register_t;

typedef struct TimerStruct_t {
    const timerName_t NAME;
    const uint32_t BASE_ADDR;
    register_t controlRegister;
    register_t intervalLoadRegister;
    register_t interruptClearRegister;
    bool isInit;
} TimerStruct_t;

// clang-format off
static TimerStruct_t TIMER_POOL[6] = {
    { TIMER0, TIMER0_BASE, (register_t) (TIMER0_BASE + CTRL), (register_t) (TIMER0_BASE + INTERVAL), (register_t) (TIMER0_BASE + INT_CLEAR), false },
    { TIMER1, TIMER1_BASE, (register_t) (TIMER1_BASE + CTRL), (register_t) (TIMER1_BASE + INTERVAL), (register_t) (TIMER1_BASE + INT_CLEAR), false },
    { TIMER2, TIMER2_BASE, (register_t) (TIMER2_BASE + CTRL), (register_t) (TIMER2_BASE + INTERVAL), (register_t) (TIMER2_BASE + INT_CLEAR), false },
    { TIMER3, TIMER3_BASE, (register_t) (TIMER3_BASE + CTRL), (register_t) (TIMER3_BASE + INTERVAL), (register_t) (TIMER3_BASE + INT_CLEAR), false },
    { TIMER4, TIMER4_BASE, (register_t) (TIMER4_BASE + CTRL), (register_t) (TIMER4_BASE + INTERVAL), (register_t) (TIMER4_BASE + INT_CLEAR), false },
    { TIMER5, TIMER5_BASE, (register_t) (TIMER5_BASE + CTRL), (register_t) (TIMER5_BASE + INTERVAL), (register_t) (TIMER5_BASE + INT_CLEAR), false }
};

// clang-format on

/******************************************************************************
Initialization
*******************************************************************************/

Timer_t Timer_Init(timerName_t timerName) {
    Timer_t timer = &TIMER_POOL[timerName];
    if(timer->isInit == false) {
        // Start clock to timer
        SYSCTL_RCGCTIMER_R |= (1 << timerName);
        while((SYSCTL_PRTIMER_R & (1 << timerName)) == 0) {}
        timer->isInit = true;
    }

    // Disable timers and turn on concatenated mode
    *timer->controlRegister &= ~(0x0101);
    *((register_t) (timer->BASE_ADDR + CONFIG)) &= ~(0x0007);

    return timer;
}

timerName_t Timer_getName(Timer_t timer) {
    Assert(timer->isInit);
    return timer->NAME;
}

bool Timer_isInit(Timer_t timer) {
    return timer->isInit;
}

/******************************************************************************
Configuration
*******************************************************************************/

void Timer_setMode(Timer_t timer, timerMode_t timerMode, timerDirection_t timerDirection) {
    Assert(timer->isInit);
    *timer->controlRegister &= ~(0x101);                                    // disable timer

    *((register_t) (timer->BASE_ADDR + MODE)) &= ~(0x13);
    switch(timerMode) {
        case ONESHOT:
            *((register_t) (timer->BASE_ADDR + MODE)) |= 0x01;
            break;
        case PERIODIC:
            *((register_t) (timer->BASE_ADDR + MODE)) |= 0x02;
            break;
    }

    switch(timerDirection) {
        case(UP):
            *((register_t) (timer->BASE_ADDR + MODE)) |= 0x10;
            break;
        case(DOWN):
            *((register_t) (timer->BASE_ADDR + MODE)) &= ~(0x10);
            break;
    }

    return;
}

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

void Timer_enableInterruptOnTimeout(Timer_t timer) {
    *timer->controlRegister &= ~(0x101);                                    // disable timer
    *timer->interruptClearRegister |= 0x01;                                 // clear int.switchlag
    *((register_t) (timer->BASE_ADDR + INT_MASK)) |= 0x01;

    return;
}

void Timer_disableInterruptOnTimeout(Timer_t timer) {
    *timer->controlRegister &= ~(0x101);                                    // disable timer
    *((register_t) (timer->BASE_ADDR + INT_MASK)) &= ~(0x01);               // disable int.
    return;
}

void Timer_clearInterruptFlag(Timer_t timer) {
    *(timer->interruptClearRegister) |= 0x01;
    return;
}

/******************************************************************************
Basic Operations
*******************************************************************************/

void Timer_setInterval_ms(Timer_t timer, uint32_t time_ms) {
    Assert(timer->isInit);
    Assert((time_ms > 0) && (time_ms <= 53000));

    *timer->controlRegister &= ~(0x101);               // disable timer
    uint32_t reload_val = (80000 * time_ms) - 1;
    *timer->intervalLoadRegister = reload_val;

    return;
}

uint32_t Timer_getCurrentValue(Timer_t timer) {
    Assert(timer->isInit);

    return *((register_t) timer->BASE_ADDR + VALUE);
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
    return (bool) (*timer->controlRegister & 0x101);
}

void Timer_Wait1ms(Timer_t timer, uint32_t time_ms) {
    Assert(timer->isInit);

    Timer_setInterval_ms(timer, time_ms);
    Timer_Start(timer);
    while(Timer_isCounting(timer)) {}

    return;
}

/** @} */
