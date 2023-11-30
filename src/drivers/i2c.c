/**
 * @addtogroup spi
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for serial peripheral interface (SPI) module.
 */

#include "i2c.h"

#include "GPIO.h"

#include "NewAssert.h"

#include "tm4c123gh6pm.h"

#include <stdbool.h>
#include <stdint.h>

enum GPIO_PORT_BASE_ADDRESSES {
    GPIO_PORTA_BASE_ADDRESS = (uint32_t) 0x40004000,
    GPIO_PORTB_BASE_ADDRESS = (uint32_t) 0x40005000,
    GPIO_PORTC_BASE_ADDRESS = (uint32_t) 0x40006000,
    GPIO_PORTD_BASE_ADDRESS = (uint32_t) 0x40007000,
    GPIO_PORTE_BASE_ADDRESS = (uint32_t) 0x40024000,
    GPIO_PORTF_BASE_ADDRESS = (uint32_t) 0x40025000,
};

enum I2C_BASE_ADDRESSES {
    I2C0_BASE_ADDR = (uint32_t) 0x40020000,
    I2C1_BASE_ADDR = (uint32_t) 0x40021000,
    I2C2_BASE_ADDR = (uint32_t) 0x40022000,
    I2C3_BASE_ADDR = (uint32_t) 0x40023000,
};

enum I2C_REGISTER_OFFSETS {
    MSTR_SUB_ADDR = (uint32_t) 0x00,
    MSTR_CTRL_STAT = (uint32_t) 0x04,
    MSTR_DATA = (uint32_t) 0x08,
    MSTR_TMR_PRD = (uint32_t) 0x0C,
    MSTR_INT_MASK = (uint32_t) 0x10,
    MSTR_RAW_INT_STAT = (uint32_t) 0x14,
    MSTR_INT_CLEAR = (uint32_t) 0x1C,
    MSTR_CONFIG = (uint32_t) 0x20,
    MSTR_CLK_LOW_TIMEOUT_COUNT = (uint32_t) 0x24,
    MSTR_BUS_MONITOR = (uint32_t) 0x2C,
    MSTR_CONFIG_2 = (uint32_t) 0x38,
};

typedef struct I2C_Struct_t {
    const uint32_t BASE_ADDRESS;
    bool isInit;
} I2C_Struct_t;

static I2C_Struct_t I2C_POOL[4] = { { I2C0_BASE_ADDR, false },
                                    { I2C1_BASE_ADDR, false },
                                    { I2C2_BASE_ADDR, false },
                                    { I2C3_BASE_ADDR, false } };

I2C_t I2C_Init(GpioPort_t gpioPort, I2C_Num i2cNum) {
    assert(GPIO_isPortInit(gpioPort));

    // check GPIO pins
    uint32_t gpio_baseAddress = GPIO_getBaseAddr(gpioPort);
    GpioPin_t clkPin;
    GpioPin_t dataPin;

    switch(i2cNum) {
        case I2C0:
            assert(gpio_baseAddress == GPIO_PORTB_BASE_ADDRESS);
            clkPin = GPIO_PIN2;
            dataPin = GPIO_PIN3;
            break;
        case I2C1:
            assert(gpio_baseAddress == GPIO_PORTA_BASE_ADDRESS);
            clkPin = GPIO_PIN6;
            dataPin = GPIO_PIN7;
            break;
        case I2C2:
            assert(gpio_baseAddress == GPIO_PORTE_BASE_ADDRESS);
            clkPin = GPIO_PIN4;
            dataPin = GPIO_PIN5;
            break;
        case I2C3:
            assert(gpio_baseAddress == GPIO_PORTD_BASE_ADDRESS);
            clkPin = GPIO_PIN0;
            dataPin = GPIO_PIN1;
            break;
        default:
            assert(false);
    }

    I2C_t i2c = &I2C_POOL[i2cNum];
    if(i2c->isInit == false) {
        // config. GPIO pins
        GPIO_ConfigAltMode(gpioPort, clkPin | dataPin);
        GPIO_ConfigPortCtrl(gpioPort, clkPin | dataPin, 3);
        GPIO_configOpenDrain(gpioPort, dataPin, true);
        GPIO_EnableDigital(gpioPort, clkPin | dataPin);

        // enable clock to I2C, and wait for it to be ready
        SYSCTL_RCGCI2C_R |= (1 << i2cNum);
        while((SYSCTL_PRI2C_R & (1 << i2cNum)) == 0) {}

        //...
        i2c->isInit = true;
    }
    return i2c;
}

bool I2C_isInit(I2C_t i2c) {
    return i2c->isInit;
}

uint16_t I2C_Read(I2C_t i2c) {
    assert(i2c->isInit);
    //...
}

/** @} */               // spi
