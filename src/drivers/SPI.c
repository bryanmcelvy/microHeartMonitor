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

#include "SPI.h"

#include "tm4c123gh6pm.h"

#include <stdint.h>

void SPI_Init(void) {
    /**
     *  The bit rate `BR` is set using the clock prescale divisor `CPSDVSR`
     *  and `SCR` field in the SSI Control 0 (`CR0`) register:
     * 
     *  \f$ BR = f_{bus} / ( CPSDVSR * (1 + SCR) ) \f$
     * 
     *  The ILI9341 driver has a min. read cycle of 150 [ns]
     *  and a min. write cycle of 100 [ns], so the bit rate `BR` is set to be 
     *  equal to the bus frequency
     *  (\f$ f_{bus} = 80 [MHz] \f$) divided by 12, allowing a bit rate of 
     *  6.67 [MHz], or a period of 150 [ns].
     */

    SYSCTL_RCGCSSI_R |= 0x01;                       // enable SSI0 clk.
    if ((SYSCTL_RCGCGPIO_R & 0x01) == 0) {
        SYSCTL_RCGCGPIO_R |= 0x01;                  // enable GPIO Port A clk.
    }

    GPIO_PORTA_AFSEL_R |= 0x3C;                     // alt. mode for PA2-5
    GPIO_PORTA_PCTL_R |= 0x3C;                      // SSI mode for PA2-5
    
    GPIO_PORTA_DIR_R |= 0xC0;                       // use PA6/7 as output pins

    GPIO_PORTA_AMSEL_R &= ~(0xFC);                  // disable analog for PA2-7
    GPIO_PORTA_DEN_R |= 0xFC;                       // enable digital IO for PA2-7
    GPIO_PORTA_DATA_R |= 0x80;                      // set `RESET` signal `HIGH` (active `LOW`)

    SSI0_CR1_R &= ~(0x02);                          // disable SSI0
    SSI0_CR1_R &= ~(0x04);                          // controller (M) mode
    SSI0_CC_R &= ~(0x0F);                           // system clock
    SSI0_CPSR_R |= 0x06;                            
    SSI0_CR0_R &= ~(0xFFFF);                        // clk. phase = 0, clk. polarity = 0, SPI mode
    SSI0_CR0_R |= 0x0107;                           // SCR = 1, 8-bit data
    SSI0_CR1_R |= 0x02;                             // re-enable SSI0
}

uint8_t SPI_Read(void) {
    while (SSI0_SR_R & 0x04) {}                     // wait until Rx FIFO is empty
    return (uint8_t) (SSI0_DR_R & 0xFF);            // return data from data register
}

void SPI_WriteCmd(uint8_t cmd) {
    while (SSI0_SR_R & 0x10) {}                     // wait until SSI0 is ready
    GPIO_PORTA_DATA_R &= ~(0x40);                   // clear D/C to write command
    SSI0_DR_R = cmd;                                // write command
    while (SSI0_SR_R & 0x10) {}                     // wait until transmission is finished
}

void SPI_WriteData(uint8_t data) {
    while ((SSI0_SR_R & 0x02) == 0) {}              // wait until Tx FIFO isn't full
    GPIO_PORTA_DATA_R |= 0x40;                      // set D/C to write data
    SSI0_DR_R += data;                              // write command
}

void SPI_WriteSequence(uint8_t cmd, uint8_t * param_sequence, uint8_t num_params) {
    if (cmd != 0) { SPI_WriteCmd(cmd); }
    for (uint8_t i = 0; i < num_params; i++) {
        SPI_WriteData( *(param_sequence+i) );
    }
}

/** @} */
