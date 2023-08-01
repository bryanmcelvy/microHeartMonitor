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
    uint16_t start_address, uint16_t end_address, bool is_row);

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
    uint16_t start_address, uint16_t end_address, bool is_row) {
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

void ILI9341_sleepMode(bool is_sleeping) {
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

void ILI9341_setVertScrollArea( uint16_t top_fixed, 
                                uint16_t vert_scroll,
                                uint16_t bottom_fixed) {
//TODO: Write
}

void ILI9341_setVertScrollStart(uint16_t start_address) {
    //TODO: Write
}

void ILI9341_setMemAccessCtrl(  bool areRowsFlipped, bool areColsFlipped,
                                bool areRowsColsSwitched, bool isVertRefreshFlipped,
                                bool isColorOrderFlipped, bool isHorRefreshFlipped) {
    /**
    *   This function implements the "Memory Access Control" (`MADCTL`) command from
    *   p. 127-128 of the ILI9341 datasheet, which controls how the LCD driver
    *   displays data upon writing to memory.
    *
    *   Name | Bit # | Effect when set = `1`
    *   -----|-------|-------------------------------------------
    *   MY   |  7    | flip row (AKA "page") addresses
    *   MX   |  6    | flip column addresses
    *   MV   |  5    | exchange rows and column addresses
    *   ML   |  4    | reverse horizontal refresh order
    *   BGR  |  3    | reverse color input order (RGB -> BGR)
    *   MH   |  2    | reverse vertical refresh order
    *
    *   All bits are clear after powering on or `HWRESET`.
    */

    uint8_t param;

    // start with all bits cleared
    param = 0x00;

    // set bits based on input arguments
    param = (areRowsFlipped) ? (param | 0x80) : param;
    param = (areColsFlipped) ? (param | 0x40) : param;
    param = (areRowsColsSwitched) ? (param | 0x20) : param;
    param = (isVertRefreshFlipped) ? (param | 0x10) : param;
    param = (isColorOrderFlipped) ? (param | 0x08) : param;
    param = (isHorRefreshFlipped) ? (param | 0x04) : param;

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
    /**
    *   This function implements the "Interface Control" `IFCTL` command from
    *   p. 192-194 of the ILI9341 datasheet, which controls how the LCD driver
    *   handles 16-bit data and what interfaces (internal or external) are used.
    *
    *       Name | Bit # | Param # | Effect when set = `1`
    *   ---------|-------|---------|-------------------------------------------
    *   MY_EOR   |  7    |    0    | flips value of corresponding MADCTL bit
    *   MX_EOR   |  6    |    ^    | flips value of corresponding MADCTL bit
    *   MV_EOR   |  5    |    ^    | flips value of corresponding MADCTL bit
    *   BGR_EOR  |  3    |    ^    | flips value of corresponding MADCTL bit
    *   WEMODE   |  0    |    ^    | overflowing pixel data is not ignored
    *   EPF[1:0] |  5:4  |    1    | controls 16 to 18-bit pixel data conversion
    *   MDT[1:0] |  1:0  |    ^    | controls display data transfer method
    *   ENDIAN   |  5    |    2    | host sends LSB first
    *   DM[1:0]  |  3:2  |    ^    | selects display operation mode
    *   RM       |  1    |    ^    | selects GRAM interface mode
    *   RIM      |  0    |    ^    | specifies RGB interface-specific details
    *
    *   The first param's bits are cleared so that the corresponding MADCTL bits
    *   (ILI9341_setMemoryAccessCtrl()) are unaffected and overflowing pixel
    *   data is ignored. The EPF bits are cleared so that the LSB of the
    *   R and B values is copied from the MSB when using 16-bit color depth. 
    *   The TM4C123 sends the MSB first, so the ENDIAN bit is cleared. The other
    *   bits are cleared and/or irrelevant since the RGB and VSYNC interfaces aren't used.
    */

    const uint8_t param_sequence[3] = {0x00, 0x00, 0x00};
    SPI_WriteSequence(IFCTL, (uint8_t (*)) param_sequence, 3);
}
