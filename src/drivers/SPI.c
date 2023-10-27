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

#include "tm4c123gh6pm.h"
#include <stdbool.h>
#include <stdint.h>

#define SPI_IS_BUSY      (SSI0_SR_R & 0x10)
#define SPI_TX_ISNOTFULL (SSI0_SR_R & 0x02)

enum {
    // SSI0 pins
    SPI_CLK_PIN = GPIO_PIN2,
    SPI_CS_PIN = GPIO_PIN3,
    SPI_RX_PIN = GPIO_PIN4,
    SPI_TX_PIN = GPIO_PIN5,

    // GPIO pins
    SPI_DC_PIN = GPIO_PIN6,
    SPI_RESET_PIN = GPIO_PIN7,

    // pin groups
    SPI_SSI0_PINS = (SPI_CLK_PIN | SPI_CS_PIN | SPI_RX_PIN | SPI_TX_PIN),
    SPI_GPIO_PINS = (SPI_DC_PIN | SPI_RESET_PIN),
    SPI_ALL_PINS = (SPI_SSI0_PINS | SPI_GPIO_PINS)
};

static register_t gpioPortReg = 0;

void SPI_Init(void) {
    /**
     *  The bit rate `BR` is set using the (positive, even-numbered) clock
     *  prescale divisor `CPSDVSR` and the `SCR` field in the SSI Control 0 (`CR0`) register:
     *
     *  \f$ BR = f_{bus} / ( CPSDVSR * (1 + SCR) ) \f$
     *
     *  The ILI9341 driver has a min. read cycle of 150 [ns]
     *  and a min. write cycle of 100 [ns], so the bit rate `BR` is set to be
     *  equal to the bus frequency
     *  (\f$ f_{bus} = 80 [MHz] \f$) divided by 8, allowing a bit rate of
     *  10 [MHz], or a period of 100 [ns].
     */

    // activate SSI0 clock and wait for it to be ready
    SYSCTL_RCGCSSI_R |= 1;
    while((SYSCTL_PRSSI_R & 0x01) == 0) {}

    // configure GPIO pins
    GpioPort_t portA = GPIO_InitPort(A);
    GPIO_ConfigAltMode(portA, SPI_SSI0_PINS);                   // alt. mode for PA2-5
    GPIO_ConfigPortCtrl(portA, SPI_SSI0_PINS, 2);               // SSI mode for PA2-5
    GPIO_ConfigDirOutput(portA, SPI_GPIO_PINS);                 // use PA6/7 as output pins
    GPIO_EnableDigital(portA, SPI_ALL_PINS);

    gpioPortReg = GPIO_getDataRegister(portA);
    *gpioPortReg |= SPI_RESET_PIN;                              // hold `HIGH` (i.e. active `LOW`)

    // configure SSI0
    SSI0_CR1_R &= ~(0x02);                   // disable SSI0
    SSI0_CR1_R &= ~(0x15);                   /* controller (M) mode, interrupt when Tx
                                               FIFO is half-empty, no loopback to RX */
    SSI0_CC_R &= ~(0x0F);                    // system clock
    SSI0_CPSR_R = (SSI0_CPSR_R & ~(0xFF)) | 4;
    SSI0_CR0_R &= ~(0xFFFF);                 // clk. phase = 0, clk. polarity = 0, SPI mode
    SSI0_CR0_R |= 0x0107;                    // SCR = 1, 8-bit data

    SSI0_CR1_R |= 0x02;                      // re-enable SSI0

    return;
}

uint8_t SPI_Read(void) {
    while(SSI0_SR_R & 0x04) {}               // wait until Rx FIFO is empty
    return (uint8_t) (SSI0_DR_R & 0xFF);               // return data from data register
}

void SPI_WriteCmd(uint8_t cmd) {
    while(SPI_IS_BUSY) {}
    *gpioPortReg &= ~(SPI_DC_PIN);                     // signal incoming command to peripheral
    SSI0_DR_R = cmd;
    while(SPI_IS_BUSY) {}                              // wait for transmission to finish

    return;
}

void SPI_WriteData(uint8_t data) {
    while(SPI_TX_ISNOTFULL == 0) {}
    *gpioPortReg |= SPI_DC_PIN;                        // signal incoming data to peripheral
    SSI0_DR_R = data;                                  // write command

    return;
}

/** @} */
