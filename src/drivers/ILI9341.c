/**
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for ILI9341 module.
 */

#include "ILI9341.h"

void ILI9341_Init(void) {
    SPI_Init();
    Timer2A_Init();
}

void ILI9341_ResetHard(void) {
    /** The ILI9341's RESET signal requires a negative logic (i.e. active `LOW`) signal
     *  for >= 10 [us] and an additional 5 [ms] before further commands can be sent.     */

    while(Timer2A_isCounting());                // in case previous command started timer
    GPIO_PORTA_DATA_R &= ~(0x80);                    // clear PA7 for 1 [ms] to initiate a hardware reset
    Timer2A_Wait1ms(1);
    GPIO_PORTA_DATA_R |= 0x80;                       // set PA7 to end reset pulse
    Timer2A_Start(5);                           // wait 5 [ms] before next command after reset
}

void ILI9341_ResetSoft(void) {
    /** The ILI9341 requires an additional 5 [ms] before 
     *  further commands can be sent after a reset.      */
    
    while(Timer2A_isCounting());                // in case previous command started timer
    SPI_WriteCmd( (uint8_t) 0x21);
    Timer2A_Start(5);                           // wait 5 [ms] before next command after reset
}

void ILI9341_DisplayOn(uint8_t is_ON) {
    /** `0x28` for OFF, `0x29` for ON */
    is_ON = (is_ON > 1) ? 1 : is_ON;

    while(Timer2A_isCounting());                // in case previous command started timer
    SPI_WriteCmd( (uint8_t) (0x28 & is_ON) );
}

void ILI9341_SetDispInterface(void) {
    /** 
     *  This function sets the display interface according to the following table,
     *  adapted from pg. 154 of the ILI9341 datasheet.
     * 
     *  --------------------------------------------------------------------
     *  Bit   ||      7      |   6    |    5   | 4 |  3   |  2   |  1  |  0
     *  Value || ByPass_MODE | RCM[1] | RCM[0] | 0 | VSPL | HSPL | DPL | EPL
     *  --------------------------------------------------------------------
     *  ByPass_MODE: display data path; 0 for shift register, 1 for memory
     *          RCM: RGB interface selection; `10` for `DE`, `11` for `SYNC` (NOTE: use `SYNC` to set blanking porch w/o `DE` bit/signal)
     *    VPSL/HSPL: polarity for VSYNC/HSYNC; `0` for low level sync clock, `1` for high level
     *          DPL: DOTCLK polarity (i.e. when to fetch data according to the dot clock); `0` for rising edge, `1` to falling edge
     *          EPL: DE polarity for RGB interface; `0` for high enable, `1` for low enable (NOTE: irrelevant in `SYNC` mode)
     * 
     *  The default values are `01000001` (`0x41`). This function alters the RGB interface to use `SYNC` mode, 
     *  allowing the blanking porch to be set using the `0xB5` command and ignoring the value of the `DE` signal.
     */
    
    const uint8_t cmd = 0xB0;
    const uint8_t param[1] = {0x61};

    while(Timer2A_isCounting());                // in case previous command started timer
    SPI_WriteSequence(cmd, param, 1);
}

void ILI9341_SetFrameRate(uint8_t frame_rate) {

}
