/**
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for ILI9341 module.
 */

#include "ILI9341.h"

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
    ILI9341_sleepMode(0);

    ILI9341_setInterface();
    ILI9341_setRGBInterface(0x61);
    ILI9341_setPixelFormat(1);

    ILI9341_setDisplayStatus(0);
}

void ILI9341_ResetHard(void) {
    /** The ILI9341's RESET signal requires a negative logic (i.e. active `LOW`) signal
     *  for >= 10 [us] and an additional 5 [ms] before further commands can be sent.     */
    GPIO_PORTA_DATA_R &= ~(0x80);               // clear PA7 for 1 [ms] to initiate a hardware reset
    Timer2A_Wait1ms(1);
    GPIO_PORTA_DATA_R |= 0x80;                  // set PA7 to end reset pulse
    Timer2A_Wait1ms(5);                         // wait 5 [ms] after reset before next command
}

void ILI9341_ResetSoft(void) {
    /** The ILI9341 requires an additional 5 [ms] before 
     *  further commands can be sent after a reset.      */
    SPI_WriteCmd(SWRESET);
    Timer2A_Wait1ms(5);                         // wait 5 [ms] after reset before next command
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

void ILI9341_setRowAddress(uint16_t start_row, uint16_t end_row) {
    /**
        This function implements the "Page Address Set" command from
        p. 112 of the ILI9341 datasheet. 
        
        The input parameters represent the first and last rows to be written
        to when ILI9341_WriteMem() is called.

        To work correctly, `start_row` must be no greater than `end_row`,
        and `end_row` cannot be greater than 319.
    */
    
    uint8_t cmd_sequence[4];

    // ensure `start_row` and `end_row` meet restrictions
    end_row = (end_row < NUM_ROWS) ? end_row : (NUM_ROWS - 1);
    start_row = (start_row > end_row) ? end_row : start_row;

    // configure send command sequence
    cmd_sequence[0] = (uint8_t) ((start_row & 0x1100) >> 8);
    cmd_sequence[1] = (uint8_t) (start_row & 0x0011);
    cmd_sequence[2] = (uint8_t) ((end_row & 0x1100) >> 8);
    cmd_sequence[3] = (uint8_t) (end_row & 0x0011);
    SPI_WriteSequence(PASET, cmd_sequence, 4);
}

void ILI9341_setColAddress(uint16_t start_col, uint16_t end_col) {
    /**
        This function implements the "Column Address Set" command from
        p. 110 of the ILI9341 datasheet. 
        
        The input parameters represent the first and last columns to be written
        to when ILI9341_WriteMem() is called.

        To work correctly, `start_col` must be no greater than `end_col`,
        and `end_col` cannot be greater than 239.
    */
    
    uint8_t cmd_sequence[4];

    end_col = (end_col < NUM_COLS) ? end_col : (NUM_COLS - 1);
    start_col = (start_col > end_col) ? end_col : start_col;

    cmd_sequence[0] = (uint8_t) ((start_col & 0x1100) >> 8);
    cmd_sequence[1] = (uint8_t) (start_col & 0x0011);
    cmd_sequence[2] = (uint8_t) ((end_col & 0x1100) >> 8);
    cmd_sequence[3] = (uint8_t) (end_col & 0x0011);

    SPI_WriteSequence(CASET, cmd_sequence, 4);
}

void ILI9341_writeMemCmd(void){
    ///TODO: Write Description
    SPI_WriteCmd(RAMWR);
}

void ILI9341_write1px(uint8_t red, uint8_t green, uint8_t blue) {     
    ///TODO: Write Description
    uint8_t data[2];

    data[0] = ((red & 0x1F) << 3) |
                ((green & 0x38) >> 3);
    data[1] = ((green & 0x07) << 5) |
                (blue & 0x1F);
    SPI_WriteSequence(0, data, 2);
}

//TODO: readMem

/**********************************************************************
Display Config.
***********************************************************************/

void ILI9341_sleepMode(uint8_t is_sleeping) {
    /**     This function turns sleep mode ON or OFF 
     *      depending on the value of `is_sleeping`.
     *      Either way, the MCU must wait >= 5 [ms]
     *      before sending further commands.
     *
     *      It's also necessary to wait 120 [ms] before
     *      sending `SPLOUT` after sending `SPLIN` or a reset,
     *      so this function waits 120 [ms] regardless of the preceding event.
     */
    Timer2A_Wait1ms(120);

    if (is_sleeping) { SPI_WriteCmd(SPLIN); }
    else { SPI_WriteCmd(SPLOUT); }
}

void ILI9341_setDispInversion(uint8_t is_ON) {
    is_ON = (is_ON > 1) ? 1 : is_ON;
    if (is_ON) { SPI_WriteCmd(DINVON); }
    else { SPI_WriteCmd(DINVOFF); }
    SPI_WriteCmd(NOP);
}

void ILI9341_setDisplayStatus(uint8_t is_ON) {
    is_ON = (is_ON > 1) ? 1 : is_ON;
    if (is_ON) { SPI_WriteCmd(DISPON); }
    else { SPI_WriteCmd(DISPOFF); }
    SPI_WriteCmd(NOP);
}

//TODO: Write
void ILI9341_setVertScrollArea( uint16_t top_fixed, 
                                uint16_t vert_scroll,
                                uint16_t bottom_fixed) {}

//TODO: Write
void ILI9341_setVertScrollStart(uint16_t start_address) {}

//TODO: Write
void ILI9341_setMemAccessCtrl(  
        uint8_t row_address_order, uint8_t col_address_order,
        uint8_t row_col_exchange, uint8_t vert_refresh_order,
        uint8_t rgb_order, uint8_t hor_refresh_order) {}

void ILI9341_setPixelFormat(uint8_t is_16bit) {
    uint8_t param = (is_16bit) ? 0x55 : 0x66;
    SPI_WriteCmd(PIXSET);
    SPI_WriteData(param);
}

void ILI9341_setDispBrightness(uint8_t brightness) {
    SPI_WriteCmd(WRDISBV);
    SPI_WriteData(brightness);
}

//TODO: Write
uint8_t ILI9341_getDispBrightness(void) {    
    return 0;
}


/**********************************************************************
Other
***********************************************************************/

void ILI9341_NoOpCmd(void) {
    SPI_WriteCmd(NOP);
}

void ILI9341_setRGBInterface(uint8_t param) {
/** 
*  This function sets the RGB interface according to the following table,
*  adapted from pg. 154 of the ILI9341 datasheet.
* 
*  Bit    |      7      |   6    |    5   | 4 |  3   |  2   |  1  |  0
*  -------|-------------|--------|--------|---|------|------|-----|-----
*  Value  | ByPass_MODE | RCM[1] | RCM[0] | 0 | VSPL | HSPL | DPL | EPL
*  Default|      0      |   1    |    0   | 0 |   0  |   0  |  0  |  1
*  
*  Name        | Description           | 0                 | 1             | Notes
*  ------------|-----------------------|-------------------|---------------|--------------------------------------------------------
*  ByPass_MODE | display data path     | shift register    | memory        | N/A
*       RCM[1] | RGB interface select  | N/A               | N/A           | Always `1`
*       RCM[0] | RGB interface select  | `DE`              |`SYNC`         | use `SYNC` to set blanking porch w/o `DE` bit/signal
*         VPSL | VSYNC polarity        | low level         | high level    | N/A
*         HSPL | HSYNC polarity        | low level         | high level    | N/A
*          DPL | DOTCLK polarity       | rising edge       | falling edge  | when to fetch data relative to the dot clock
*          EPL | DE polarity           | high enable       | low enable    | irrelevant in `SYNC` mode
*/
    
    // const uint8_t param[1] = {0x61}; use `SYNC` mode
    SPI_WriteCmd(IFMODE);
    SPI_WriteData(param);
}

//TODO: Write
void ILI9341_setFrameRate(uint8_t div_ratio, uint8_t clocks_per_line) {

}

//TODO: Write
void ILI9341_setBlankingPorch(uint8_t vert_front_porch, uint8_t vert_back_porch,
                                uint8_t hor_front_porch, uint8_t hor_back_porch) {
}

void ILI9341_setInterface(void) { //TODO: Add comments/make nicer
    /// RGB Interface, 6-bit data transfer (2-3 transfer/pixel)
    uint8_t cmd_sequence[3] = {0x01, 0x00, 0x03};
    SPI_WriteSequence(IFCTL, cmd_sequence, 3);
}
