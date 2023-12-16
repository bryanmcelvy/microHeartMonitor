/**
 * @addtogroup  timer
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for Timer module.
 */

#include "Timer.h"

#include "sysctrl.h"

#include "NewAssert.h"

#include "m-profile/cmsis_gcc_m.h"
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

typedef struct TimerStruct_t {
    timerName_t name;
    uint32_t baseAddress;
    register_t controlRegister;
    register_t intervalLoadRegister;
    register_t interruptClearRegister;
    bool * isInit;
} TimerStruct_t;

static bool initStatusArray[6] = { false, false, false, false, false, false };

// clang-format off
static const TimerStruct_t TIMER_STRUCT_ARRAY[6] = {
    { TIMER0, TIMER0_BASE, REGISTER_CAST(TIMER0_BASE + CTRL), REGISTER_CAST(TIMER0_BASE + INTERVAL), REGISTER_CAST(TIMER0_BASE + INT_CLEAR), &initStatusArray[0] },
    { TIMER1, TIMER1_BASE, REGISTER_CAST(TIMER1_BASE + CTRL), REGISTER_CAST(TIMER1_BASE + INTERVAL), REGISTER_CAST(TIMER1_BASE + INT_CLEAR), &initStatusArray[1] },
    { TIMER2, TIMER2_BASE, REGISTER_CAST(TIMER2_BASE + CTRL), REGISTER_CAST(TIMER2_BASE + INTERVAL), REGISTER_CAST(TIMER2_BASE + INT_CLEAR), &initStatusArray[2] },
    { TIMER3, TIMER3_BASE, REGISTER_CAST(TIMER3_BASE + CTRL), REGISTER_CAST(TIMER3_BASE + INTERVAL), REGISTER_CAST(TIMER3_BASE + INT_CLEAR), &initStatusArray[3] },
    { TIMER4, TIMER4_BASE, REGISTER_CAST(TIMER4_BASE + CTRL), REGISTER_CAST(TIMER4_BASE + INTERVAL), REGISTER_CAST(TIMER4_BASE + INT_CLEAR), &initStatusArray[4] },
    { TIMER5, TIMER5_BASE, REGISTER_CAST(TIMER5_BASE + CTRL), REGISTER_CAST(TIMER5_BASE + INTERVAL), REGISTER_CAST(TIMER5_BASE + INT_CLEAR), &initStatusArray[5] }
};

// clang-format on

/******************************************************************************
Initialization
*******************************************************************************/

Timer_t Timer_Init(timerName_t timerName) {
    Timer_t timer = &TIMER_STRUCT_ARRAY[timerName];
    if(*timer->isInit == false) {
        SysCtrl_configPeripheralClk(SYSCTRL_RUN, SYSCTRL_TIMER, timerName, SYSCTRL_CLK_ON);
        *timer->isInit = true;
    }

    // Disable timers and turn on concatenated mode
    *timer->controlRegister &= ~(0x0101);
    REGISTER_VAL(timer->baseAddress + CONFIG) &= ~(0x0007);

    return timer;
}

void Timer_Deinit(Timer_t timer) {
    if(*timer->isInit) {
        *timer->controlRegister &= ~(0x101);               // stop timer

        // disable clock to timer
        uint8_t timerNum = timer->name;
        SysCtrl_configPeripheralClk(SYSCTRL_RUN, SYSCTRL_TIMER, timerNum, SYSCTRL_CLK_OFF);
        *timer->isInit = false;
    }
    return;
}

timerName_t Timer_getName(Timer_t timer) {
    assert(*timer->isInit);
    return timer->name;
}

bool Timer_isInit(Timer_t timer) {
    return *timer->isInit;
}

/******************************************************************************
Configuration
*******************************************************************************/

void Timer_setMode(Timer_t timer, timerMode_t timerMode, timerDirection_t timerDirection) {
    assert(*timer->isInit);
    *timer->controlRegister &= ~(0x101);                                  // disable timer

    REGISTER_VAL(timer->baseAddress + MODE) &= ~(0x13);
    switch(timerMode) {
        case ONESHOT:
            REGISTER_VAL(timer->baseAddress + MODE) |= 0x01;
            break;
        case PERIODIC:
            REGISTER_VAL(timer->baseAddress + MODE) |= 0x02;
            break;
    }

    switch(timerDirection) {
        case(UP):
            REGISTER_VAL(timer->baseAddress + MODE) |= 0x10;
            break;
        case(DOWN):
            REGISTER_VAL(timer->baseAddress + MODE) &= ~(0x10);
            break;
    }

    return;
}

void Timer_enableAdcTrigger(Timer_t timer) {
    assert(*timer->isInit);

    *timer->controlRegister |= 0x20;
    return;
}

void Timer_disableAdcTrigger(Timer_t timer) {
    assert(*timer->isInit);

    *timer->controlRegister &= ~(0x20);
    return;
}

void Timer_enableInterruptOnTimeout(Timer_t timer) {
    *timer->controlRegister &= ~(0x101);                                  // disable timer
    *timer->interruptClearRegister |= 0x01;                               // clear int. flag
    REGISTER_VAL(timer->baseAddress + INT_MASK) |= 0x01;
    return;
}

void Timer_disableInterruptOnTimeout(Timer_t timer) {
    *timer->controlRegister &= ~(0x101);                                  // disable timer
    REGISTER_VAL(timer->baseAddress + INT_MASK) &= ~(0x01);               // disable int.
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
    assert(*timer->isInit);
    assert((time_ms > 0) && (time_ms <= 53000));

    *timer->controlRegister &= ~(0x101);               // disable timer
    uint32_t reload_val = (80000 * time_ms) - 1;
    *timer->intervalLoadRegister = reload_val;

    return;
}

uint32_t Timer_getCurrentValue(Timer_t timer) {
    assert(*timer->isInit);

    return REGISTER_VAL(timer->baseAddress + VALUE);
}

void Timer_Start(Timer_t timer) {
    assert(*timer->isInit);

    *timer->controlRegister |= 0x101;                  // enable timer
    return;
}

void Timer_Stop(Timer_t timer) {
    assert(*timer->isInit);

    *timer->controlRegister &= ~(0x101);               // stop/disable timer
    return;
}

bool Timer_isCounting(Timer_t timer) {
    return (bool) (*timer->controlRegister & 0x101);
}

void Timer_Wait1ms(Timer_t timer, uint32_t time_ms) {
    assert(*timer->isInit);

    Timer_setInterval_ms(timer, time_ms);
    Timer_Start(timer);
    while(Timer_isCounting(timer)) {
        __NOP();
    }

    return;
}

/** @} */
