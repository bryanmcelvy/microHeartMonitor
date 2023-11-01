/**
 * @addtogroup spi
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for serial peripheral interface (SPI) module.
 */

#include "SPI.h"

#include "GPIO.h"

#include "NewAssert.h"

#include "tm4c123gh6pm.h"

#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
Constant Declarations
*******************************************************************************/

enum GPIO_PORT_BASE_ADDRESSES {
    GPIO_PORTA_BASE_ADDRESS = (uint32_t) 0x40004000,
    GPIO_PORTB_BASE_ADDRESS = (uint32_t) 0x40005000,
    GPIO_PORTC_BASE_ADDRESS = (uint32_t) 0x40006000,
    GPIO_PORTD_BASE_ADDRESS = (uint32_t) 0x40007000,
    GPIO_PORTE_BASE_ADDRESS = (uint32_t) 0x40024000,
    GPIO_PORTF_BASE_ADDRESS = (uint32_t) 0x40025000,
};

enum SSI_BASE_ADDRESSES {
    SSI0_BASE_ADDR = (uint32_t) 0x40008000,
    SSI1_BASE_ADDR = (uint32_t) 0x40009000,
    SSI2_BASE_ADDR = (uint32_t) 0x4000A000,
    SSI3_BASE_ADDR = (uint32_t) 0x4000B000,
};

enum SSI_REGISTER_OFFSETS {
    CTRL0_OFFSET = (uint32_t) 0,
    CTRL1_OFFSET = (uint32_t) 0x004,
    DATA_OFFSET = (uint32_t) 0x008,
    STATUS_OFFSET = (uint32_t) 0x00C,
    CLK_PRESCALE_OFFSET = (uint32_t) 0x010,
    INT_MASK_OFFSET = (uint32_t) 0x014,
    RAW_INT_STATUS_OFFSET = (uint32_t) 0x018,
    MASKED_INT_STATUS_OFFSET = (uint32_t) 0x01C,
    INT_CLEAR_OFFSET = (uint32_t) 0x020,
};

/******************************************************************************
Initialization
*******************************************************************************/

typedef struct SpiStruct_t {
    const uint32_t BASE_ADDRESS;
    volatile uint32_t * const DATA_REGISTER;
    volatile uint32_t * const STATUS_REGISTER;

    volatile uint32_t * gpioDataRegister;
    GpioPin_t gpioDataCommPin;

    uint8_t dataSize;
    bool isEnabled;
    bool isInit;
} SpiStruct_t;

static SpiStruct_t SPI_ARR[4] = {
    { SSI0_BASE_ADDR, (volatile uint32_t *) (SSI0_BASE_ADDR + DATA_OFFSET),
      (volatile uint32_t *) (SSI0_BASE_ADDR + STATUS_OFFSET), 0, 0, 0, false, false },
    { SSI1_BASE_ADDR, (volatile uint32_t *) (SSI1_BASE_ADDR + DATA_OFFSET),
      (volatile uint32_t *) (SSI1_BASE_ADDR + STATUS_OFFSET), 0, 0, 0, false, false },
    { SSI2_BASE_ADDR, (volatile uint32_t *) (SSI2_BASE_ADDR + DATA_OFFSET),
      (volatile uint32_t *) (SSI2_BASE_ADDR + STATUS_OFFSET), 0, 0, 0, false, false },
    { SSI3_BASE_ADDR, (volatile uint32_t *) (SSI3_BASE_ADDR + DATA_OFFSET),
      (volatile uint32_t *) (SSI3_BASE_ADDR + STATUS_OFFSET), 0, 0, 0, false, false },
};

Spi_t SPI_Init(GpioPort_t gpioPort, GpioPin_t dcPin, SsiNum_t ssiNum) {
    Assert(GPIO_isPortInit(gpioPort));
    Assert(dcPin <= GPIO_PIN7);

    // check GPIO pins
    uint32_t gpio_baseAddress = GPIO_getBaseAddr(gpioPort);
    GpioPin_t gpioPins;

    switch(ssiNum) {
        case SSI0:
            Assert(gpio_baseAddress == GPIO_PORTA_BASE_ADDRESS);
            gpioPins = GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5;
            break;
        case SSI1:
            Assert(gpio_baseAddress == GPIO_PORTF_BASE_ADDRESS);
            gpioPins = GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3;
            break;
        case SSI2:
            Assert(gpio_baseAddress == GPIO_PORTB_BASE_ADDRESS);
            gpioPins = GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7;
            break;
        case SSI3:
            Assert(gpio_baseAddress == GPIO_PORTD_BASE_ADDRESS);
            gpioPins = GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3;
            break;
        default:
            Assert(false);
    }

    Assert((dcPin & gpioPins) == 0);

    // initialize SSI peripheral in SPI mode
    Spi_t spi = &SPI_ARR[ssiNum];
    if(spi->isInit == false) {
        // config. GPIO pins
        GPIO_ConfigAltMode(gpioPort, gpioPins);
        GPIO_ConfigPortCtrl(gpioPort, gpioPins, 2);

        GPIO_configDirection(gpioPort, dcPin, GPIO_OUTPUT);

        GPIO_EnableDigital(gpioPort, gpioPins | dcPin);

        // enable clock to SSI, and wait for it to be ready
        SYSCTL_RCGCSSI_R |= (1 << ssiNum);
        while((SYSCTL_PRSSI_R & (1 << ssiNum)) == 0) {}

        // config control registers
        register_t ctrlRegister0 = (register_t) (spi->BASE_ADDRESS + CTRL0_OFFSET);
        register_t ctrlRegister1 = (register_t) (spi->BASE_ADDRESS + CTRL1_OFFSET);
        register_t clkPrescaleReg = (register_t) (spi->BASE_ADDRESS + CLK_PRESCALE_OFFSET);

        *ctrlRegister1 &= ~(0x02);               // disable
        *ctrlRegister1 &= ~(0x15);               // controller (master) mode, no EOT, no loopback

        *ctrlRegister0 &= ~(0x30);               // SPI frame format

        // set bit rate to 10 [MHz]
        *clkPrescaleReg = (*clkPrescaleReg & ~(0xFF)) | 4;
        *ctrlRegister0 = (*ctrlRegister0 & ~(0xFF00)) | (0x0100);

        spi->gpioDataRegister = GPIO_getDataRegister(gpioPort);
        spi->gpioDataCommPin = dcPin;
        spi->isEnabled = false;
        spi->isInit = true;
    }

    return spi;
}

bool SPI_isInit(Spi_t spi) {
    return spi->isInit;
}

/******************************************************************************
Configuration
*******************************************************************************/

void SPI_configClock(Spi_t spi, SpiClockPhase_t clockPhase, SpiClockPolarity_t clockPolarity) {
    Assert(spi->isInit);
    Assert(spi->isEnabled == false);

    register_t ctrlRegister0 = (register_t) (spi->BASE_ADDRESS + CTRL0_OFFSET);

    switch(clockPhase) {
        case SPI_RISING_EDGE:
            *ctrlRegister0 &= ~(1 << 7);
            break;
        case SPI_FALLING_EDGE:
            *ctrlRegister0 |= (1 << 7);
            break;
        default:
            Assert(false);
    }

    switch(clockPolarity) {
        case SPI_STEADY_STATE_LOW:
            *ctrlRegister0 &= ~(1 << 6);
            break;
        case SPI_STEADY_STATE_HIGH:
            *ctrlRegister0 |= (1 << 6);
            break;
        default:
            Assert(false);
    }

    return;
}

void SPI_setDataSize(Spi_t spi, uint8_t dataSize) {
    Assert(spi->isInit);
    Assert(spi->isEnabled == false);
    Assert(dataSize >= 4);
    Assert(dataSize <= 16);

    register_t ctrlRegister0 = (register_t) (spi->BASE_ADDRESS + CTRL0_OFFSET);
    *ctrlRegister0 = (*ctrlRegister0 & ~(0x0F)) | (dataSize - 1);

    spi->dataSize = dataSize;
    return;
}

void SPI_Enable(Spi_t spi) {
    Assert(spi->isInit);
    Assert(spi->dataSize > 0);
    if(spi->isEnabled == false) {
        register_t ctrlRegister1 = (register_t) (spi->BASE_ADDRESS + CTRL1_OFFSET);
        *ctrlRegister1 |= 0x02;
        spi->isEnabled = true;
    }
    return;
}

void SPI_Disable(Spi_t spi) {
    Assert(spi->isInit);
    if(spi->isEnabled) {
        register_t ctrlRegister1 = (register_t) (spi->BASE_ADDRESS + CTRL1_OFFSET);
        *ctrlRegister1 &= ~(0x02);
        spi->isEnabled = false;
    }
}

/******************************************************************************
Operations
*******************************************************************************/

uint16_t SPI_Read(Spi_t spi) {
    Assert(spi->isInit);
    Assert(spi->isEnabled);

    return *spi->DATA_REGISTER;
}

void SPI_WriteCmd(Spi_t spi, uint16_t cmd) {
    Assert(spi->isInit);
    Assert(spi->isEnabled);

    while(*spi->STATUS_REGISTER & 0x10) {}                           // wait while SPI is busy

    *spi->gpioDataRegister &= ~(spi->gpioDataCommPin);               // signal incoming command
    *spi->DATA_REGISTER = cmd & ((1 << spi->dataSize) - 1);

    while(*spi->STATUS_REGISTER & 0x10) {}                        // wait for transmission to finish
    return;
}

void SPI_WriteData(Spi_t spi, uint16_t data) {
    Assert(spi->isInit);
    Assert(spi->isEnabled);

    while((*spi->STATUS_REGISTER & 0x02) == 0) {}                 // wait while TX FIFO is full

    *spi->gpioDataRegister |= spi->gpioDataCommPin;               // signal incoming data
    *spi->DATA_REGISTER = data & ((1 << spi->dataSize) - 1);

    return;
}

/** @} */                                                         // spi
