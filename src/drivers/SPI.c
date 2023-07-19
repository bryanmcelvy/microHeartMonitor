/**
 *  Pin |  Function |   Bus
 *  --------------------------------------------------------------
 *  PA2 |  SSI0Clk  |   CLK     |   Serial clock signal
 *  PA3 |  SSI0Fss  |   CS      |   Chip select signal
 *  PA5 |  SSI0Tx   |   MOSI    |   TM4C (M) output, LCD (S) input
 *  PA6 |  GPIO     |   D/C     |   Data = 1, Command = 0
 *  PA7 |  GPIO     |   RESET   |   Reset the display
 * 
 *  Clk. Polarity  =   steady state low (0)
 *  Clk. Phase     =   rising clock edge (0)
 */

#include "SPI.h"

void SPI_Init(void) {
    /**
     *  @details
     *      
     *  The bit rate `BR` is set using the clock prescale divisor `CPSDVSR`
     *  and `SCR` field in the SSI Control 0 (`CR0`) register:
     * 
     *  $\fBR = f_{bus} / ( CPSDVSR * (1 + SCR) )$\f
     * 
     *  The ILI9341 driver has a minimum write cycle of 100 [ns],
     *  corresponding to a maximum serial clock frequency of 10 [MHz].
     */

    SYSCTL_RCGCSSI_R |= 0x01;                       // enable SSI0 clk.
    SYSCTL_RCGCGPIO_R |= 0x01;                      // enable Port A clk.

    GPIO_PORTA_AFSEL_R |= 0x2C;                     // alt. mode for PA2/3/5
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0)     // SSI mode for PA2/3/5
                        | 0x2C;
    
    GPIO_PORTA_DIR_R |= 0xC0;                       // use PA6/7 as output pins

    GPIO_PORTA_AMSEL_R &= ~(0xEC);                  // disable analog for PA2/3/5-7
    GPIO_PORTA_DEN_R |= 0xEC;                       // enable digital IO for PA2/3/5-7

    SSI0_CR1_R &= ~(0x02);                          // disable SSI0
    SSI0_CR1_R &= ~(0x04);                          // controller (M) mode
    SSI0_CC_R &= ~(0x0F);                           // system clock
    SSI0_CPSR_R |= 0x0A;                            // = 10
    SSI0_CR0_R = ( SSI0_CR0_R & ~(0xFFF0) )         // SCR = 0, clk. phase = 0,
                        | 0x07;                     // clk. polarity = 0, SPI, 8-bit data
    SSI0_CR1_R |= 0x02;                             // re-enable SSI0
}
