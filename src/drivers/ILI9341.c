/**
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for ILI9341 module.
 */

#include "ILI9341.h"

#include "SPI.h"
#include "Timer.h"

#include "tm4c123gh6pm.h"

#include <stdint.h>
#include <stdbool.h>

static void ILI9341_setAddress(
    uint16_t start_address, uint16_t end_address, uint8_t is_row);

/**********************************************************************
Initialization/Reset
***********************************************************************/

void ILI9341_Init(void) {
    ///TODO: rewrite description
    
    SPI_Init();
    Timer2A_Init();

    ILI9341_ResetHard();
    ILI9341_sleepMode(0);

    ILI9341_setInterface();
    ILI9341_setColorDepth(1);

    ILI9341_setDisplayStatus(1);
}

void ILI9341_ResetHard(void) {
    /** 
     *  The LCD driver's RESET pin requires a negative logic (i.e. active `LOW`)
     *  signal for >= 10 [us] and an additional 5 [ms] before further commands 
     *  can be sent.
     */
    GPIO_PORTA_DATA_R &= ~(0x80);               // clear PA7 to init. reset
    Timer2A_Wait1ms(1);
    GPIO_PORTA_DATA_R |= 0x80;                  // set PA7 to end reset pulse
    Timer2A_Wait1ms(5);
}

void ILI9341_ResetSoft(void) {
    SPI_WriteCmd(SWRESET);
    Timer2A_Wait1ms(5); /// the driver needs 5 [ms] before another command
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

static void ILI9341_setAddress(
    uint16_t start_address, uint16_t end_address, uint8_t is_row) {
    /**
    This function implements the "Column Address Set" (`CASET`) and "Page
    Address Set" (`PASET`) commands from p. 110-113 of the ILI9341 datasheet.
    
    The input parameters represent the first and last addresses to be written
    to when ILI9341_write1px() is called.

    To work correctly, `start_address` must be no greater than `end_address`,
    and `end_address` cannot be greater than the max number of rows/columns.
    */
    
    uint8_t cmd_sequence[4];

    uint8_t cmd = (is_row) ? PASET : CASET;
    uint16_t max_num = (is_row) ? NUM_ROWS : NUM_COLS;

    // ensure `start_address` and `end_address` meet restrictions
    end_address = (end_address < max_num) ? end_address : (max_num - 1);
    start_address = (start_address < end_address) ? start_address : end_address;

    // configure send command sequence
    cmd_sequence[0] = (uint8_t) ((start_address & 0xFF00) >> 8);
    cmd_sequence[1] = (uint8_t) (start_address & 0x00FF);
    cmd_sequence[2] = (uint8_t) ((end_address & 0xFF00) >> 8);
    cmd_sequence[3] = (uint8_t) (end_address & 0x00FF);
    SPI_WriteSequence(cmd, cmd_sequence, 4);
}

void ILI9341_setRowAddress(uint16_t start_row, uint16_t end_row) {
    /**
        This function is simply an interface to ILI9341_setAddress().
        To work correctly, `start_row` must be no greater than `end_row`, and
        `end_row` cannot be greater than the max row number (default 320).
    */
    
    ILI9341_setAddress(start_row, end_row, 1);
}

void ILI9341_setColAddress(uint16_t start_col, uint16_t end_col) {
    /**
        This function is simply an interface to ILI9341_setAddress().
        To work correctly, `start_col` must be no greater than `end_col`, and
        `end_col` cannot be greater than the max column number (default 240).
    */
    
    ILI9341_setAddress(start_col, end_col, 0);
}

void ILI9341_writeMemCmd(void){
    /**
     *  Sends the "Write Memory" (`RAMWR`) command to the LCD driver,
     *  signalling that incoming data should be written to memory. 
     */
    SPI_WriteCmd(RAMWR);
}

void ILI9341_write1px(uint8_t red, uint8_t green, uint8_t blue, bool is_16bit) {     
    ///TODO: Write Description
    
    uint8_t data[3] = {0};

    if (is_16bit) {
        data[0] = ((red & 0x1F) << 3) |
                    ((green & 0x38) >> 3);
        data[1] = ((green & 0x07) << 5) |
                    (blue & 0x1F);
        SPI_WriteSequence(0, data, 2);
    }
    else {
        data[0] = ( (red & 0x3F) << 2 ) + 0x03;
        data[1] = ( (green & 0x3F) << 2 ) + 0x03;
        data[2] = ( (blue & 0x3F) << 2 ) + 0x03;
        SPI_WriteSequence(0, data, 3);
    }
}

///TODO: readMem

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

void ILI9341_setDispInversion(bool is_ON) {
    ///TODO: Write description
    if (is_ON) { SPI_WriteCmd(DINVON); }
    else { SPI_WriteCmd(DINVOFF); }
}

void ILI9341_setDisplayStatus(bool is_ON) {
    ///TODO: Write description
    if (is_ON) { SPI_WriteCmd(DISPON); }
    else { SPI_WriteCmd(DISPOFF); }
}

//TODO: Write
void ILI9341_setVertScrollArea( uint16_t top_fixed, 
                                uint16_t vert_scroll,
                                uint16_t bottom_fixed) {}

//TODO: Write
void ILI9341_setVertScrollStart(uint16_t start_address) {}

//TODO: Write
void ILI9341_setMemAccessCtrl(  uint8_t row_order, uint8_t col_order,
        uint8_t row_col_exchange, uint8_t vert_refresh_order,
                                uint8_t rgb_order, uint8_t hor_refresh_order) {
    uint8_t param = 0x00;

    param = (row_order > 0) ? (param | 0x80) : param;
    param = (col_order > 0) ? (param | 0x40) : param;
    param = (row_col_exchange > 0) ? (param | 0x20) : param;
    param = (vert_refresh_order > 0) ? (param | 0x10) : param;
    param = (rgb_order > 0) ? (param | 0x08) : param;
    param = (hor_refresh_order > 0) ? (param | 0x04) : param;

    SPI_WriteCmd(MADCTL);
    SPI_WriteData(param);
}

void ILI9341_setColorDepth(bool is_16bit) {
    /**
     *  16-bit requires 2 transfers and allows for 65K colors.
     *  18-bit requires 3 transfers and allows for 262K colors.
     */
    uint8_t param = (is_16bit) ? 0x55 : 0x66;
    SPI_WriteCmd(PIXSET);
    SPI_WriteData(param);
}

/**********************************************************************
Other
***********************************************************************/

void ILI9341_NoOpCmd(void) {
    SPI_WriteCmd(NOP);
}

//TODO: Write
void ILI9341_setFrameRate(uint8_t div_ratio, uint8_t clocks_per_line) {

}

//TODO: Write
void ILI9341_setBlankingPorch(  uint8_t vpf, uint8_t vbp, 
                                uint8_t hfp, uint8_t hbp) {
}

void ILI9341_setInterface(void) { 
    //TODO: Write description

    uint8_t cmd_sequence[3] = {0x01, 0x00, 0x00};
    SPI_WriteSequence(IFCTL, cmd_sequence, 3);
}
