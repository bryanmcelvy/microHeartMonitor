/**
 * @addtogroup sysctrl
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Header file for System Control module.
 */

#include "sysctrl.h"

#include "NewAssert.h"

#include "m-profile/cmsis_gcc_m.h"
#include "tm4c123gh6pm.h"

#include <stdbool.h>
#include <stdint-gcc.h>

/******************************************************************************
Phase-Locked Loop (PLL)
*******************************************************************************/

static uint8_t PLL_Divisor;

void SysCtrl_PLL_Init(uint8_t divisor) {
    assert(divisor > 4);
    assert(divisor != 7);
    assert(divisor <= 128);

    // Disable PLL and system clock divider
    SYSCTL_RCC_R &= ~(SYSCTL_RCC_USESYSDIV);               // disable system clock divider
    SYSCTL_RCC2_R |= SYSCTL_RCC2_USERCC2;               // use RCC2 register for more freq. options
    SYSCTL_RCC2_R |= SYSCTL_RCC2_BYPASS2;               // set BYPASS2 to disable PLL

    // Select crystal value and oscillator source
    SYSCTL_RCC_R = (SYSCTL_RCC_R & ~(SYSCTL_RCC_XTAL_M))               // 16[MHz] crystal
                   | SYSCTL_RCC_XTAL_16MHZ;
    SYSCTL_RCC2_R &= ~(SYSCTL_RCC2_OSCSRC2_M);                         // use main oscillator
    SYSCTL_RCC2_R &= ~(SYSCTL_RCC2_PWRDN2);                            // power on PLL

    // Set system clock divider
    SYSCTL_RCC2_R |= SYSCTL_RCC2_DIV400;                                 // enable 7-bit divisor
    SYSCTL_RCC2_R &=
        ~(SYSCTL_RCC2_SYSDIV2_M | SYSCTL_RCC2_SYSDIV2LSB);               // clear divisor bits
    SYSCTL_RCC2_R |= ((divisor - 1) << 22);                              // = (f_PLL / f_bus) - 1
    SYSCTL_RCC_R |= SYSCTL_RCC_USESYSDIV;               // enable system clock divider

    // Re-activate PLL
    while((SYSCTL_RIS_R & SYSCTL_RIS_PLLLRIS) == 0) {               // wait for PLL to lock
        __NOP();
    }
    SYSCTL_RCC2_R &= ~(SYSCTL_RCC2_BYPASS2);                        // clear BYPASS2 to enable PLL

    PLL_Divisor = divisor;

    return;
}

uint32_t SysCtrl_getClkFreq_Hz(void) {
    static uint32_t freq = 16000000;

    // Calculate and return PLL frequency if it's initialized
    if(SYSCTL_PLLSTAT_R) {
        uint32_t MINT = SYSCTL_PLLFREQ0_R & SYSCTL_PLLFREQ0_MINT_M;
        uint32_t MFRAC = (SYSCTL_PLLFREQ0_R & SYSCTL_PLLFREQ0_MFRAC_M) >> 10;
        uint32_t MDIV = MINT + (MFRAC / 1024);

        uint32_t Q = (SYSCTL_PLLFREQ1_R & SYSCTL_PLLFREQ1_Q_M) >> 8;
        uint32_t N = SYSCTL_PLLFREQ1_R & SYSCTL_PLLFREQ1_N_M;

        freq = (16 * MDIV) / ((Q + 1) * (N + 1));               // PLL frequency
        freq /= PLL_Divisor;                                    // clock frequency
    }

    return freq;
}

/******************************************************************************
Clock Gating Control
*******************************************************************************/

#define SYSCTRL_BASE ((uint32_t) 0x400FE000)

enum MODE_REGISTER_OFFSETS {
    RUN_OFFSET = (uint32_t) 0x600,                      ///< Run Mode Clock Gate Control
    SLEEP_OFFSET = (uint32_t) 0x700,                    ///< Sleep Mode Clock Gate Control
    DEEP_SLEEP_OFFSET = (uint32_t) 0x800,               ///< Deep Sleep Mode Clock Gate Control
    READY_OFFSET = (uint32_t) 0xA00,                    ///< Peripheral Ready
};

enum PERIPHERAL_REGISTER_OFFSETS {
    TIMER_OFFSET = (uint32_t) 0x04,
    GPIO_OFFSET = (uint32_t) 0x08,
    DMA_OFFSET = (uint32_t) 0x0C,
    UART_OFFSET = (uint32_t) 0x18,
    SSI_OFFSET = (uint32_t) 0x1C,
    I2C_OFFSET = (uint32_t) 0x20,
    ADC_OFFSET = (uint32_t) 0x38,
    PWM_OFFSET = (uint32_t) 0x40,
};

void SysCtrl_configPeripheralClk(SysCtrlClkMode_e mode, SysCtrlPeripheral_e peripheral, uint8_t num,
                                 SysCtrlClkStatus_e status) {
    // Determine register offsets for the mode and peripheral
    uint32_t modeOffset;
    switch(mode) {
        case SYSCTRL_RUN:
            modeOffset = RUN_OFFSET;
            break;
        case SYSCTRL_SLEEP:
            modeOffset = SLEEP_OFFSET;
            break;
        case SYSCTRL_DEEP_SLEEP:
            modeOffset = DEEP_SLEEP_OFFSET;
            break;
        default:
            assert(false);
    }

    uint32_t peripheralOffset;
    switch(peripheral) {
        case SYSCTRL_ADC:
            assert(num < 2);
            peripheralOffset = ADC_OFFSET;
            break;
        case SYSCTRL_DMA:
            assert(num == 0);
            peripheralOffset = DMA_OFFSET;
            break;
        case SYSCTRL_GPIO:
            assert(num < 6);
            peripheralOffset = GPIO_OFFSET;
            break;
        case SYSCTRL_I2C:
            assert(num < 4);
            peripheralOffset = I2C_OFFSET;
            break;
        case SYSCTRL_PWM:
            assert(num < 2);
            peripheralOffset = PWM_OFFSET;
            break;
        case SYSCTRL_SSI:
            assert(num < 4);
            peripheralOffset = SSI_OFFSET;
            break;
        case SYSCTRL_TIMER:
            assert(num < 6);
            peripheralOffset = TIMER_OFFSET;
            break;
        case SYSCTRL_UART:
            assert(num < 8);
            peripheralOffset = UART_OFFSET;
            break;
        default:
            assert(false);
    }

    switch(status) {
        case SYSCTRL_CLK_ON:               // clang-format off
            // Start clock to peripheral and wait for it to be ready
            REGISTER_VAL(SYSCTRL_BASE + modeOffset + peripheralOffset) |= (1 << num);
            while(((REGISTER_VAL(SYSCTRL_BASE + READY_OFFSET + peripheralOffset)) & (1 << num)) == 0) {
                __NOP();
            }
            break;               // clang-format on
        case SYSCTRL_CLK_OFF:
            // Stop clock to peripheral and allow it to turn off
            REGISTER_VAL(SYSCTRL_BASE + modeOffset + peripheralOffset) &= ~(1 << num);
            while((REGISTER_VAL(SYSCTRL_BASE + READY_OFFSET + peripheralOffset)) & (1 << num)) {
                __NOP();
            }
            break;
        default:
            assert(false);
    }

    return;
}

/******************************************************************************
Sleep Mode
*******************************************************************************/

#define CM4_SYSCTRL_R         (REGISTER_VAL(0xE000ED10))
#define CM4_SYSCTRL_SEVONPEND ((uint32_t) 0x10)
#define CM4_SYSCTRL_SLEEPDEEP ((uint32_t) 0x04)
#define CM4_SYSCTRL_SLEEPXIT  ((uint32_t) 0x02)

void SysCtrl_configSleepMode(SysCtrlSleepConfig_e param) {
    switch(param) {
        case SYSCTRL_CFG_WAKE_ON_ENABLED:
            CM4_SYSCTRL_R &= ~(CM4_SYSCTRL_SEVONPEND);
            break;
        case SYSCTRL_CFG_WAKE_ON_PEND:
            CM4_SYSCTRL_R |= CM4_SYSCTRL_SEVONPEND;
            break;
        case SYSCTRL_CFG_NORMAL_SLEEP:
            CM4_SYSCTRL_R &= ~(CM4_SYSCTRL_SLEEPDEEP);
            break;
        case SYSCTRL_CFG_DEEP_SLEEP:
            CM4_SYSCTRL_R |= CM4_SYSCTRL_SLEEPDEEP;
            break;
        case SYSCTRL_CFG_NO_SLEEP_ON_EXIT:
            CM4_SYSCTRL_R &= ~(CM4_SYSCTRL_SLEEPXIT);
            break;
        case SYSCTRL_CFG_SLEEP_ON_EXIT:
            CM4_SYSCTRL_R |= CM4_SYSCTRL_SLEEPXIT;
            break;
        default:
            assert(false);
    }
}

void SysCtrl_enterSleepMode(SysCtrlSleepModes_e sleepMode) {
    switch(sleepMode) {
        case WAIT_FOR_INT:
            __WFI();
            break;
        case WAIT_FOR_EVENT:
            __WFE();
            break;
        default:
            assert(false);
    }
    return;
}

/******************************************************************************/

/** @} */               // sysctrl
