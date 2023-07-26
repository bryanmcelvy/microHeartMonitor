/**
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for ILI9341 module.
 */

#include "ILI9341.h"
#include "SPI.h"
#include <stdint.h>

/**********************************************************************
Initialization/Reset
***********************************************************************/

void ILI9341_Init(void) {
    /**
     *  This function initializes the SPI (i.e. SSI0) and Timer2A peripherals,
     *  initiates a hardware reset of the display, and tunes the display interface
     *  to allow blanking porch values to be manipulated via SPI commands.
     */
    
    SPI_Init();
    Timer2A_Init();

    ILI9341_ResetHard();
    ILI9341_setDisplay(0);
}

void ILI9341_ResetHard(void) {
    /** The ILI9341's RESET signal requires a negative logic (i.e. active `LOW`) signal
     *  for >= 10 [us] and an additional 5 [ms] before further commands can be sent.     */

    while(Timer2A_isCounting());                // in case previous command started timer
    GPIO_PORTA_DATA_R &= ~(0x80);               // clear PA7 for 1 [ms] to initiate a hardware reset
    Timer2A_Wait1ms(1);
    GPIO_PORTA_DATA_R |= 0x80;                  // set PA7 to end reset pulse
    Timer2A_Start(5);                           // wait 5 [ms] before next command after reset
}

void ILI9341_ResetSoft(void) {
    /** The ILI9341 requires an additional 5 [ms] before 
     *  further commands can be sent after a reset.      */
    
    while(Timer2A_isCounting());                // in case previous command started timer
    SPI_WriteCmd(SWRESET);
    Timer2A_Start(5);                           // wait 5 [ms] before next command after reset
}

/**********************************************************************
Reading Display Status
***********************************************************************/

uint8_t * ILI9341_getDispStatus(void) { //TODO: Write
    return 0;
}

uint8_t ILI9341_getMemAccessCtrl(void) { //TODO: Write
    return 0;
}

//TODO getPixelFormat

/**********************************************************************
Memory Reading/Writing
***********************************************************************/

void ILI9341_setRowAddress(uint8_t start_row, uint8_t end_row) { //TODO: Write

}

void ILI9341_setColAddress(uint8_t start_col, uint8_t end_col) { //TODO: Write

}

void ILI9341_write(uint8_t data[]) { //TODO: Write

}

//TODO: readMem

/**********************************************************************
Display Config.
***********************************************************************/
void ILI9341_setDispInv(uint8_t is_ON) { //TODO: Write

}

void ILI9341_setDisplay(uint8_t is_ON) {
    /// `0x28` for OFF, `0x29` for ON
    is_ON = (is_ON > 1) ? 1 : is_ON;

    while(Timer2A_isCounting());                // in case previous command started timer
    SPI_WriteCmd( (uint8_t) (0x28 & is_ON) );
}
void ILI9341_setVertScrollArea( uint16_t top_fixed, 
                                uint16_t vert_scroll,
                                uint16_t bottom_fixed) { //TODO: Write

}
void ILI9341_setVertScrollStart(uint16_t start_address) { //TODO: Write

}

void ILI9341_setMemAccessCtrl(  
        uint8_t row_address_order, uint8_t col_address_order,
        uint8_t row_col_exchange, uint8_t vert_refresh_order,
        uint8_t rgb_order, uint8_t hor_refresh_order) { //TODO: Write
}
void ILI9341_setPixelFormat(uint8_t is_16bit) { //TODO: Write

}

void ILI9341_setDispBrightness(uint8_t brightness) { //TODO: Write

}

uint8_t ILI9341_getDispBrightness(void) { //TODO: Write
    
    return 0;
}


/**********************************************************************
Other
***********************************************************************/

void ILI9341_setDispInterface(uint8_t param) {
    /** 
     *  This function sets the display interface according to the following table,
     *  adapted from pg. 154 of the ILI9341 datasheet.
     * 
     *  --------------------------------------------------------------------
     *  Bit   ||      7      |   6    |    5   | 4 |  3   |  2   |  1  |  0
     *  Value || ByPass_MODE | RCM[1] | RCM[0] | 0 | VSPL | HSPL | DPL | EPL
     *  --------------------------------------------------------------------
     *  ByPass_MODE: display data path; 0 for shift register, 1 for memory
     *          RCM: RGB interface selection; `10` for `DE`, `11` for `SYNC`
                    (NOTE: use `SYNC` to set blanking porch w/o `DE` bit/signal)
     *    VPSL/HSPL: polarity for VSYNC/HSYNC
                    `0` for low level sync clock, `1` for high level
     *          DPL: DOTCLK polarity (i.e. when to fetch data according to the dot clock); 
                    `0` for rising edge, `1` to falling edge
     *          EPL: DE polarity for RGB interface; 
                    `0` for high enable, `1` for low enable
                    (NOTE: irrelevant in `SYNC` mode)
     * 
     *  The default values are `01000001` (`0x41`).
     */
    
    // const uint8_t param[1] = {0x61}; use `SYNC` mode

    while(Timer2A_isCounting());                // in case previous command started timer
    SPI_WriteCmd(IFMODE);
    SPI_WriteData(param);
}
void ILI9341_setFrameRate(uint8_t div_ratio, uint8_t clocks_per_line) { //TODO: Write

}

void ILI9341_setBlankingPorch(uint8_t vert_front_porch, uint8_t vert_back_porch,
                                uint8_t hor_front_porch, uint8_t hor_back_porch) { //TODO: Write
}
