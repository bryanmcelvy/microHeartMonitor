#include "sysctrl.h"

#include "NewAssert.h"

#include "m-profile/cmsis_gcc_m.h"
#include "tm4c123gh6pm.h"

#include <stdbool.h>
#include <stdint-gcc.h>

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

void SysCtrl_setClkGatingCtrl(SysCtrlClkMode_e mode, SysCtrlPeripheral_e peripheral, uint8_t num) {
    // Determine register offsets for the mode and peripheral
    uint32_t modeOffset;
    switch(mode) {
        case RUN:
            modeOffset = RUN_OFFSET;
            break;
        case SLEEP:
            modeOffset = SLEEP_OFFSET;
            break;
        case DEEP_SLEEP:
            modeOffset = DEEP_SLEEP_OFFSET;
            break;
        default:
            assert(false);
    }

    uint32_t peripheralOffset;
    switch(peripheral) {
        case ADC:
            assert(num < 2);
            peripheralOffset = ADC_OFFSET;
            break;
        case DMA:
            assert(num == 0);
            peripheralOffset = DMA_OFFSET;
            break;
        case GPIO:
            assert(num < 6);
            peripheralOffset = GPIO_OFFSET;
            break;
        case I2C:
            assert(num < 4);
            peripheralOffset = I2C_OFFSET;
            break;
        case PWM:
            assert(num < 2);
            peripheralOffset = PWM_OFFSET;
            break;
        case SSI:
            assert(num < 4);
            peripheralOffset = SSI_OFFSET;
            break;
        case TIMER:
            assert(num < 6);
            peripheralOffset = TIMER_OFFSET;
            break;
        case UART:
            assert(num < 8);
            peripheralOffset = UART_OFFSET;
            break;
        default:
            assert(false);
    }

    // Start clock to peripheral and wait for it to be ready
    REGISTER_VAL(SYSCTRL_BASE + modeOffset + peripheralOffset) |= (1 << num);
    while(((REGISTER_VAL(SYSCTRL_BASE + READY_OFFSET + peripheralOffset)) & (1 << num)) == 0) {
        __NOP();
    }

    return;
}

#define CM4_SYSCTRL_ADDR ((uint32_t) 0xE000ED10)

void SysCtrl_enterSleepMode(SysCtrlSleepModes_e sleepMode) {
    REGISTER_VAL(CM4_SYSCTRL_ADDR) &= ~(0x12);
    switch(sleepMode) {
        case WAIT_FOR_INT:
            __WFI();
            break;
        case WAIT_FOR_EVENT:
            REGISTER_VAL(CM4_SYSCTRL_ADDR) |= 0x10;
            __WFE();
            break;
        case SLEEP_ON_EXIT:
            REGISTER_VAL(CM4_SYSCTRL_ADDR) |= 0x02;
            break;
        default:
            assert(false);
    }
    return;
}

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
    SYSCTL_RCC2_R |= SYSCTL_RCC2_DIV400;                     // enable 7-bit divisor
    SYSCTL_RCC2_R &= ~(SYSCTL_RCC2_SYSDIV2_M);               // clear divisor bits
    SYSCTL_RCC2_R |= ((divisor - 1) << 22);                  // = (f_PLL / f_bus) - 1
    SYSCTL_RCC_R |= SYSCTL_RCC_USESYSDIV;                    // enable system clock divider

    // Re-activate PLL
    while((SYSCTL_RIS_R & SYSCTL_RIS_PLLLRIS) == 0) {               // wait for PLL to lock
        __NOP();
    }
    SYSCTL_RCC2_R &= ~(SYSCTL_RCC2_BYPASS2);                        // clear BYPASS2 to enable PLL

    return;
}

uint32_t SysCtrl_PLL_getFreq(void) {
    uint32_t MINT = SYSCTL_PLLFREQ0_R & SYSCTL_PLLFREQ0_MINT_M;
    uint32_t MFRAC = (SYSCTL_PLLFREQ0_R & SYSCTL_PLLFREQ0_MFRAC_M) >> 10;
    uint32_t MDIV = MINT + (MFRAC / 1024);

    uint32_t Q = (SYSCTL_PLLFREQ1_R & SYSCTL_PLLFREQ1_Q_M) >> 8;
    uint32_t N = SYSCTL_PLLFREQ1_R & SYSCTL_PLLFREQ1_N_M;

    return (16 * MDIV) / ((Q + 1) * (N + 1));
}

bool SysCtrl_PLL_isOn(void) {
    return (bool) SYSCTL_PLLSTAT_R;
}
