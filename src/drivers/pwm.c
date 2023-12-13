#include "pwm.h"

#include "GPIO.h"

#include "NewAssert.h"

#include "tm4c123gh6pm.h"

#include <stdbool.h>

/* Enumerations */

enum GPIO_PORT_BASE_ADDRESSES {
    GPIO_PORTA_BASE_ADDRESS = (uint32_t) 0x40004000,
    GPIO_PORTB_BASE_ADDRESS = (uint32_t) 0x40005000,
    GPIO_PORTC_BASE_ADDRESS = (uint32_t) 0x40006000,
    GPIO_PORTD_BASE_ADDRESS = (uint32_t) 0x40007000,
    GPIO_PORTE_BASE_ADDRESS = (uint32_t) 0x40024000,
    GPIO_PORTF_BASE_ADDRESS = (uint32_t) 0x40025000,
};

enum PWM_BASE_ADDRESSES {
    PWM0_BASE = (uint32_t) 0x40028000,
    PWM1_BASE = (uint32_t) 0x40029000
};

enum PWM_REGISTER_OFFSETS {
    MASTER_CTRL = (uint32_t) 0,
    OUTPUT_ENABLE = (uint32_t) 0x008,

    PWM0_CTRL = (uint32_t) 0x040,
    PWM0_LOAD = (uint32_t) 0x050,
    PWM0_COUNTER = (uint32_t) 0x054,
    PWM0_COMPARE_A = (uint32_t) 0x058,
    PWM0_COMPARE_B = (uint32_t) 0x05C,
    PWM0_GEN_A_CTRL = (uint32_t) 0x060,
    PWM0_GEN_B_CTRL = (uint32_t) 0x064,

    PWM1_CTRL = PWM0_CTRL + 0x40,
    PWM1_LOAD = PWM0_LOAD + 0x40,
    PWM1_COUNTER = PWM0_COUNTER + 0x40,
    PWM1_COMPARE_A = PWM0_COMPARE_A + 0x40,
    PWM1_COMPARE_B = PWM0_COMPARE_B + 0x40,
    PWM1_GEN_A_CTRL = PWM0_GEN_A_CTRL + 0x40,
    PWM1_GEN_B_CTRL = PWM0_GEN_B_CTRL + 0x40,

    PWM2_CTRL = PWM1_CTRL + 0x40,
    PWM2_LOAD = PWM1_LOAD + 0x40,
    PWM2_COUNTER = PWM1_COUNTER + 0x40,
    PWM2_COMPARE_A = PWM1_COMPARE_A + 0x40,
    PWM2_COMPARE_B = PWM1_COMPARE_B + 0x40,
    PWM2_GEN_A_CTRL = PWM1_GEN_A_CTRL + 0x40,
    PWM2_GEN_B_CTRL = PWM1_GEN_B_CTRL + 0x40,

    PWM3_CTRL = PWM2_CTRL + 0x40,
    PWM3_LOAD = PWM2_LOAD + 0x40,
    PWM3_COUNTER = PWM2_COUNTER + 0x40,
    PWM3_COMPARE_A = PWM2_COMPARE_A + 0x40,
    PWM3_COMPARE_B = PWM2_COMPARE_B + 0x40,
    PWM3_GEN_A_CTRL = PWM2_GEN_A_CTRL + 0x40,
    PWM3_GEN_B_CTRL = PWM2_GEN_B_CTRL + 0x40,
};

/* Type Declaration */

typedef struct PwmStruct_t {
    uint32_t PWM_MODULE_BASE;
    register_t genCtrlRegPtr;

    uint32_t frequency;
    uint32_t dutyCycle;
    bool isEnabled;
    bool isInit;
} PwmStruct_t;

static PwmStruct_t PWM_POOL[16] = {
    { PWM0_BASE, REGISTER_CAST(PWM0_BASE + PWM0_CTRL + GEN_A_CTRL_OFFSET), 0, 100, false, false },
    { PWM0_BASE, REGISTER_CAST(PWM0_BASE + PWM0_CTRL + GEN_B_CTRL_OFFSET), 0, 100, false, false },
    { PWM0_BASE, REGISTER_CAST(PWM0_BASE + PWM1_CTRL + GEN_A_CTRL_OFFSET), 0, 100, false, false },
    { PWM0_BASE, REGISTER_CAST(PWM0_BASE + PWM1_CTRL + GEN_B_CTRL_OFFSET), 0, 100, false, false },
    { PWM0_BASE, REGISTER_CAST(PWM0_BASE + PWM2_CTRL + GEN_A_CTRL_OFFSET), 0, 100, false, false },
    { PWM0_BASE, REGISTER_CAST(PWM0_BASE + PWM2_CTRL + GEN_B_CTRL_OFFSET), 0, 100, false, false },
    { PWM0_BASE, REGISTER_CAST(PWM0_BASE + PWM3_CTRL + GEN_A_CTRL_OFFSET), 0, 100, false, false },
    { PWM0_BASE, REGISTER_CAST(PWM0_BASE + PWM3_CTRL + GEN_B_CTRL_OFFSET), 0, 100, false, false },
    { PWM1_BASE, REGISTER_CAST(PWM1_BASE + PWM0_CTRL + GEN_A_CTRL_OFFSET), 0, 100, false, false },
    { PWM1_BASE, REGISTER_CAST(PWM1_BASE + PWM0_CTRL + GEN_B_CTRL_OFFSET), 0, 100, false, false },
    { PWM1_BASE, REGISTER_CAST(PWM1_BASE + PWM1_CTRL + GEN_A_CTRL_OFFSET), 0, 100, false, false },
    { PWM1_BASE, REGISTER_CAST(PWM1_BASE + PWM1_CTRL + GEN_B_CTRL_OFFSET), 0, 100, false, false },
    { PWM1_BASE, REGISTER_CAST(PWM1_BASE + PWM2_CTRL + GEN_A_CTRL_OFFSET), 0, 100, false, false },
    { PWM1_BASE, REGISTER_CAST(PWM1_BASE + PWM2_CTRL + GEN_B_CTRL_OFFSET), 0, 100, false, false },
    { PWM1_BASE, REGISTER_CAST(PWM1_BASE + PWM3_CTRL + GEN_A_CTRL_OFFSET), 0, 100, false, false },
    { PWM1_BASE, REGISTER_CAST(PWM1_BASE + PWM3_CTRL + GEN_B_CTRL_OFFSET), 0, 100, false, false },
};

/* Functions */

Pwm_t Pwm_Init(GpioPort_t gpioPort, PwmPin_e pwmPin) {
    assert(GPIO_isPortInit(gpioPort));
    assert(pwmPin <= M1PWM7);

    // Determine GPIO pin to use
    uint32_t gpioBase = GPIO_getBaseAddr(gpioPort);
    GpioPin_t gpioPin;
    switch(pwmPin) {
        case M0PWM0:
            assert(gpioBase == GPIO_PORTB_BASE_ADDRESS);
            gpioPin = GPIO_PIN6;
            break;
        case M0PWM1:
            assert(gpioBase == GPIO_PORTB_BASE_ADDRESS);
            gpioPin = GPIO_PIN7;
            break;
        case M0PWM2:
            assert(gpioBase == GPIO_PORTB_BASE_ADDRESS);
            gpioPin = GPIO_PIN4;
            break;
        case M0PWM3:
            assert(gpioBase == GPIO_PORTB_BASE_ADDRESS);
            gpioPin = GPIO_PIN5;
            break;
        case M0PWM4:
            assert(gpioBase == GPIO_PORTE_BASE_ADDRESS);
            gpioPin = GPIO_PIN4;
            break;
        case M0PWM5:
            assert(gpioBase == GPIO_PORTE_BASE_ADDRESS);
            gpioPin = GPIO_PIN5;
            break;
        case M0PWM6:
            switch(gpioBase) {
                case GPIO_PORTC_BASE_ADDRESS:
                    gpioPin = GPIO_PIN4;
                    break;
                case GPIO_PORTD_BASE_ADDRESS:
                    gpioPin = GPIO_PIN0;
                    break;
                default:
                    assert(false);
            }
            break;
        case M0PWM7:
            switch(gpioBase) {
                case GPIO_PORTC_BASE_ADDRESS:
                    gpioPin = GPIO_PIN5;
                    break;
                case GPIO_PORTD_BASE_ADDRESS:
                    gpioPin = GPIO_PIN1;
                    break;
                default:
                    assert(false);
            }
            break;
        case M1PWM0:
            assert(gpioBase == GPIO_PORTD_BASE_ADDRESS);
            gpioPin = GPIO_PIN0;
            break;
        case M1PWM1:
            assert(gpioBase == GPIO_PORTD_BASE_ADDRESS);
            gpioPin = GPIO_PIN1;
            break;
        case M1PWM2:
            switch(gpioBase) {
                case GPIO_PORTA_BASE_ADDRESS:
                    gpioPin = GPIO_PIN6;
                    break;
                case GPIO_PORTE_BASE_ADDRESS:
                    gpioPin = GPIO_PIN4;
                    break;
                default:
                    assert(false);
            }
            break;
        case M1PWM3:
            switch(gpioBase) {
                case GPIO_PORTA_BASE_ADDRESS:
                    gpioPin = GPIO_PIN7;
                    break;
                case GPIO_PORTE_BASE_ADDRESS:
                    gpioPin = GPIO_PIN5;
                    break;
                default:
                    assert(false);
            }
            break;
        case M1PWM4:
            assert(gpioBase == GPIO_PORTF_BASE_ADDRESS);
            gpioPin = GPIO_PIN0;
            break;
        case M1PWM5:
            assert(gpioBase == GPIO_PORTF_BASE_ADDRESS);
            gpioPin = GPIO_PIN1;
            break;
        case M1PWM6:
            assert(gpioBase == GPIO_PORTF_BASE_ADDRESS);
            gpioPin = GPIO_PIN2;
            break;
        case M1PWM7:
            assert(gpioBase == GPIO_PORTF_BASE_ADDRESS);
            gpioPin = GPIO_PIN3;
            break;
        default:
            assert(false);
    }

    Pwm_t pwm = &PWM_POOL[pwmPin];
    if(pwm->isInit == false) {
        // Initialize GPIO pin
        GPIO_DisableDigital(gpioPort, gpioPin);
        GPIO_ConfigAltMode(gpioPort, gpioPin);
        GPIO_ConfigPortCtrl(gpioPort, gpioPin, ((pwmPin < M1PWM0) ? 4 : 5));

        // Enable clock to PWM module
        uint8_t pwmModuleNum = (pwmPin < M1PWM0) ? 0 : 1;
        SYSCTL_RCGCPWM_R |= (1 << pwmModuleNum);
        while((SYSCTL_PRPWM_R & (1 << pwmModuleNum)) == 0) {}

        // Set clock source to 10 [MHz]
        SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV;
        SYSCTL_RCC_R = (SYSCTL_RCC_R & ~(0x7 << 17)) | SYSCTL_RCC_PWMDIV_8;

        // Configure PWM generator block
        uint32_t pwmModuleBase = (pwmModuleNum == 0) ? PWM0_BASE : PWM1_BASE;

        switch(pwmPin / 4) {
            case 0:
                *((register_t) pwmModuleBase + PWM0_CTRL) &= ~(0x7FFFF);
                break;
            case 1:
                *((register_t) pwmModuleBase + PWM1_CTRL) &= ~(0x7FFFF);
                break;
            case 2:
                *((register_t) pwmModuleBase + PWM2_CTRL) &= ~(0x7FFFF);
                break;
            case 3:
                *((register_t) pwmModuleBase + PWM3_CTRL) &= ~(0x7FFFF);
                break;
        }

        // Populate struct
        pwm->isInit = true;
    }

    return pwm;
}

void Pwm_setFrequency(Pwm_t pwm, uint32_t freq) {
    assert(pwm->isInit);
    // TODO
}

void Pwm_setDutyCycle(Pwm_t pwm, uint32_t dutyCycle) {
    assert(pwm->isInit);
    assert(dutyCycle <= 100);
    //...
    return;
}

uint32_t Pwm_getFrequency(Pwm_t pwm) {
    assert(pwm->isInit);
    return pwm->frequency;
}

uint32_t Pwm_getDutyCycle(Pwm_t pwm) {
    assert(pwm->isInit);
    return pwm->dutyCycle;
}

void Pwm_Enable(Pwm_t pwm) {
    assert(pwm->isInit);
    //...
}

void Pwm_Disable(Pwm_t pwm) {
    assert(pwm->isInit);
    //...
}
