#ifndef SYSCTRL_H
#define SYSCTRL_H

#include <stdbool.h>
#include <stdint-gcc.h>

typedef enum {
    ADC,
    DMA,
    GPIO,
    I2C,
    PWM,
    SSI,
    TIMER,
    UART,
} SysCtrlPeripheral_e;

typedef enum {
    RUN,
    SLEEP,
    DEEP_SLEEP
} SysCtrlClkMode_e;

void SysCtrl_setClkGatingCtrl(SysCtrlClkMode_e mode, SysCtrlPeripheral_e peripheral, uint8_t num);

typedef enum {
    WAIT_FOR_INT,
    WAIT_FOR_EVENT,
    SLEEP_ON_EXIT
} SysCtrlSleepModes_e;

void SysCtrl_enterSleepMode(SysCtrlSleepModes_e sleepMode);

void SysCtrl_PLL_Init(uint8_t divisor);

uint32_t SysCtrl_PLL_getFreq(void);

bool SysCtrl_PLL_isOn(void);

#endif
