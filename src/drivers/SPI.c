/**
 * @addtogroup spi
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for SPI module.
 */

/**
 *     TM4C Pin |  Function | ILI9341 Pin   | Description
 *  ------------|-----------|---------------|------------------------------------------------
 *          PA2 |  SSI0Clk  |   CLK         | Serial clock signal
 *          PA3 |  SSI0Fss  |   CS          | Chip select signal
 *          PA4 |  SSI0Rx   |   MISO        | TM4C (M) input, LCD (S) output
 *          PA5 |  SSI0Tx   |   MOSI        | TM4C (M) output, LCD (S) input
 *          PA6 |  GPIO     |   D/C         | Data = 1, Command = 0
 *          PA7 |  GPIO     |   RESET       | Reset the display (negative logic/active `LOW`)
 *
 *  Clk. Polarity  =   steady state low (0)     <br>
 *  Clk. Phase     =   rising clock edge (0)    <br>
 */

/******************************************************************************
SECTIONS
        Preprocessor Directives
        Initialization
        Basic Operations (Polling-based)
        Basic Operations (Interrupt-based)
        Interrupt Handler
*******************************************************************************/

/******************************************************************************
Preprocessor Directives
*******************************************************************************/

// Includes
#include "SPI.h"

#include "GPIO.h"

#include "FIFO.h"

#include "tm4c123gh6pm.h"
#include <stdbool.h>
#include <stdint.h>

// Macros
#define NVIC_SSI0_NUM    7               /// SSI0's interrupt number
#define SPI_INT_START()  (NVIC_SW_TRIG_R = (NVIC_SW_TRIG_R & ~(0xFF)) | NVIC_SSI0_NUM)

#define SPI_SET_DC()     (GPIO_PORTA_DATA_R |= 0x40)
#define SPI_CLEAR_DC()   (GPIO_PORTA_DATA_R &= ~(0x40))

#define SPI_IS_BUSY      (SSI0_SR_R & 0x10)
#define SPI_TX_ISNOTFULL ((bool) (SSI0_SR_R & 0x02))

#define SPI_BUFFER_SIZE  9               // needs to be >8

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

/******************************************************************************
Initialization
*******************************************************************************/

static volatile uint32_t SPI_Buffer[SPI_BUFFER_SIZE];
static volatile FIFO_t * SPI_fifo = 0;

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
    GPIO_Port_t * port_ptr = GPIO_InitPort(A);
    GPIO_ConfigAltMode(port_ptr, SPI_SSI0_PINS);                   // alt. mode for PA2-5
    GPIO_ConfigPortCtrl(port_ptr, SPI_SSI0_PINS, 2);               // SSI mode for PA2-5

    GPIO_ConfigDirOutput(port_ptr, SPI_GPIO_PINS);                 // use PA6/7 as output pins

    GPIO_EnableDigital(port_ptr, SPI_ALL_PINS);

    GPIO_WriteHigh(port_ptr, SPI_RESET_PIN);               // hold `HIGH` (i.e. active `LOW`)

    // configure SSI0
    SSI0_CR1_R &= ~(0x02);                 // disable SSI0
    SSI0_CR1_R &= ~(0x15);                 /* controller (M) mode, interrupt when Tx
                                             FIFO is half-empty, no loopback to RX */
    SSI0_CC_R &= ~(0x0F);                  // system clock
    SSI0_CPSR_R = (SSI0_CPSR_R & ~(0xFF)) | 4;
    SSI0_CR0_R &= ~(0xFFFF);               // clk. phase = 0, clk. polarity = 0, SPI mode
    SSI0_CR0_R |= 0x0107;                  // SCR = 1, 8-bit data

    // configure interrupt
    SPI_fifo = FIFO_Init(SPI_Buffer, SPI_BUFFER_SIZE);
    NVIC_PRI1_R |= (1 << 29);               // priority 1
    NVIC_EN0_R |= (1 << NVIC_SSI0_NUM);

    SSI0_CR1_R |= 0x02;                     // re-enable SSI0
}

/******************************************************************************
Basic Operations (Polling-based)
*******************************************************************************/

uint8_t SPI_Read(void) {
    while(SSI0_SR_R & 0x04) {}                         // wait until Rx FIFO is empty
    return (uint8_t) (SSI0_DR_R & 0xFF);               // return data from data register
}

void SPI_WriteCmd(uint8_t cmd) {
    while(SPI_IS_BUSY) {}
    SPI_CLEAR_DC();                                    // signal incoming command to peripheral
    SSI0_DR_R += cmd;
    while(SPI_IS_BUSY) {}                              // wait for transmission to finish
}

void SPI_WriteData(uint8_t data) {
    while(SPI_TX_ISNOTFULL == 0) {}
    SPI_SET_DC();                                      // signal incoming data to peripheral
    SSI0_DR_R += data;                                 // write command
}

/*
NOTE: unused due to increasing call stack usage without much benefit

void SPI_WriteSequence(uint8_t cmd, uint8_t * param_sequence, uint8_t num_params) {
    if(cmd != 0) {
        SPI_WriteCmd(cmd);
    }
    for(uint8_t i = 0; i < num_params; i++) {
        SPI_WriteData(*(param_sequence + i));
    }
}
*/

/******************************************************************************
Basic Operations (Interrupt-based)
*******************************************************************************/

void SPI_IRQ_WriteCmd(uint8_t cmd) {
    while(FIFO_isFull(SPI_fifo)) {}
    FIFO_Put(SPI_fifo, cmd);

    if(FIFO_isFull(SPI_fifo)) {
        SPI_INT_START();
    }
}

void SPI_IRQ_WriteData(uint8_t data) {
    uint16_t param;
    param = ((uint16_t) data) | 0x100;               // set bit 8 to signal as data

    while(FIFO_isFull(SPI_fifo)) {}
    FIFO_Put(SPI_fifo, param);

    if(FIFO_isFull(SPI_fifo)) {
        SPI_INT_START();
    }
}

void SPI_IRQ_StartWriting(void) {
    SPI_INT_START();
}

/******************************************************************************
Interrupt Handler
*******************************************************************************/

/**
 * @brief   Sends parameters (data or commands) over SPI via SSI0.
 *
 *      The interrupt is enabled by the `SPI_Init()` function and triggered by
 *      a call to `SPI_IRQ_StartWriting()`. The handler determines whether to
 *      signal for data or a command via the D/C pin, and then writes to the data register.
 *
 *      The interrupt is unpended at the start of the function.
 */
void SSI0_Handler(void) {
    NVIC_UNPEND0_R |= (1 << NVIC_SSI0_NUM);               // acknowledge interrupt

    while(FIFO_isEmpty(SPI_fifo) == false) {
        uint16_t param = FIFO_Get(SPI_fifo);

        if((param & 0x100)) {                             // check bit 8
            SPI_SET_DC();                                 // signal data
        }
        else {
            SPI_CLEAR_DC();                               // signal incoming command
        }
        while(SPI_IS_BUSY) {}
        SSI0_DR_R += (uint8_t) param;
    }
}

/** @} */
