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

#include "Timer.h"

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

enum REGISTER_OFFSETS {
    CONFIG = 0x00,
    MODE = 0x04,
    CONTROL = 0x0C,
    INT_MASK = 0x18,
    INT_CLEAR = 0x24,
    INTERVAL_LOAD = 0x28,
    VALUE = 0x054
};

typedef volatile uint32_t * register_t;

typedef struct TimerStruct_t {
    const uint32_t BASE_ADDR;
    register_t controlRegister;
    register_t intervalLoadRegister;
    register_t interruptClearRegister;
    bool isInit;
} TimerStruct_t;

// clang-format off
static TimerStruct_t TIMER_POOL[6] = {
    { TIMER0_BASE, (register_t) (TIMER0_BASE + CONTROL), (register_t) (TIMER0_BASE + INTERVAL_LOAD), (register_t) (TIMER0_BASE + INT_CLEAR), false },
    { TIMER1_BASE, (register_t) (TIMER1_BASE + CONTROL), (register_t) (TIMER1_BASE + INTERVAL_LOAD), (register_t) (TIMER0_BASE + INT_CLEAR), false },
    { TIMER2_BASE, (register_t) (TIMER2_BASE + CONTROL), (register_t) (TIMER2_BASE + INTERVAL_LOAD), (register_t) (TIMER0_BASE + INT_CLEAR), false },
    { TIMER3_BASE, (register_t) (TIMER3_BASE + CONTROL), (register_t) (TIMER3_BASE + INTERVAL_LOAD), (register_t) (TIMER0_BASE + INT_CLEAR), false },
    { TIMER4_BASE, (register_t) (TIMER4_BASE + CONTROL), (register_t) (TIMER4_BASE + INTERVAL_LOAD), (register_t) (TIMER0_BASE + INT_CLEAR), false },
    { TIMER5_BASE, (register_t) (TIMER5_BASE + CONTROL), (register_t) (TIMER5_BASE + INTERVAL_LOAD), (register_t) (TIMER0_BASE + INT_CLEAR), false }
};

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
    *((register_t) (timer->BASE_ADDR + CONFIG)) &= ~(0x0007);

    // Configure timer according to input args
    *((register_t) (timer->BASE_ADDR + MODE)) &= ~(0x0FFF);

    if(isCountingUp) {
        *((register_t) (timer->BASE_ADDR + MODE)) |= 0x10;
    }
    else {
        *((register_t) (timer->BASE_ADDR + MODE)) &= ~(0x10);
    }

    if(isPeriodic) {
        *((register_t) (timer->BASE_ADDR + MODE)) |= 0x02;
    }
    else {
        *((register_t) (timer->BASE_ADDR + MODE)) |= 0x01;
    }

    return timer;
}

/******************************************************************************
Configuration
*******************************************************************************/

void Timer_setInterval_ms(Timer_t timer, uint32_t time_ms) {
    Assert(timer->isInit);
    Assert((time_ms > 0) && (time_ms <= 53000));

    *timer->controlRegister &= ~(0x101);                               // disable timer
    uint32_t reload_val = (80000 * time_ms) - 1;
    *timer->intervalLoadRegister = reload_val;

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

void Timer_enableInterruptOnTimeout(Timer_t timer, uint8_t priority) {
    *timer->controlRegister &= ~(0x101);                               // disable timer

    *((register_t) timer->BASE_ADDR + INT_MASK) |= 0x01;               // int. on timeout

    // enable in NVIC
    uint8_t vectorNum;
    switch(timer->BASE_ADDR) {
        case TIMER0_BASE: vectorNum = INT_TIMER0A; break;
        case TIMER1_BASE: vectorNum = INT_TIMER1A; break;
        case TIMER2_BASE: vectorNum = INT_TIMER2A; break;
        case TIMER3_BASE: vectorNum = INT_TIMER3A; break;
        case TIMER4_BASE: vectorNum = INT_TIMER4A; break;
        case TIMER5_BASE: vectorNum = INT_TIMER5A; break;
    }
    ISR_setPriority(vectorNum, priority);
    ISR_Enable(vectorNum);

    return;
}

void Timer_disableInterruptOnTimeout(Timer_t timer) {
    *timer->controlRegister &= ~(0x101);                                  // disable timer
    *((register_t) timer->BASE_ADDR + INT_MASK) &= ~(0x01);               // disable int.
    return;
}

void Timer_clearInterruptFlag(Timer_t timer) {
    *(timer->interruptClearRegister) |= 0x01;
    return;
}

/******************************************************************************
Basic Operations
*******************************************************************************/

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
