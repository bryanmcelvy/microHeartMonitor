/**
 * @addtogroup  ili9341
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for ILI9341 module.
 */

#include "ILI9341.h"

/******************************************************************************
SECTIONS
        Static Declarations
        Initialization/Reset
        Configuration
        Memory Writing
*******************************************************************************/

#include "SPI.h"
#include "Timer.h"

#include "FIFO.h"
#include "NewAssert.h"

#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
Static Declarations
*******************************************************************************/

static void ILI9341_setMode(uint8_t param);

inline static void ILI9341_setAddress(uint16_t start_address, uint16_t end_address, bool is_row);

/**
 * @brief           Send a command and/or the data within the FIFO buffer.
 *                  A command is only sent when `cmd != NOP` (where `NOP = 0`).
 *                  Data is only sent if the FIFO buffer is not empty.
 *
 * @param[in] cmd   Command to send.
 */
inline static void ILI9341_sendParams(Cmd_t cmd);

static uint32_t ILI9341_Buffer[8];
static Fifo_t ILI9341_Fifo;

static struct {
    sleepMode_t sleepMode;
    displayArea_t displayArea;
    colorExpr_t colorExpression;
    invertMode_t invertMode;
    outputMode_t outputMode;
    colorDepth_t colorDepth;

    bool isInit;
} ili9341 = { SLEEP_ON, NORMAL_AREA, FULL_COLORS, INVERT_OFF, OUTPUT_ON, COLORDEPTH_16BIT, false };

/******************************************************************************
Initialization/Reset
*******************************************************************************/

void ILI9341_Init(Timer_t timer) {
    Assert(ili9341.isInit == false);
    Assert(timer);

    ILI9341_Fifo = FIFO_Init(ILI9341_Buffer, 8);

    SPI_Init();

    ILI9341_resetHard(timer);
    ILI9341_setInterface();

    ili9341.isInit = true;
    return;
}

void ILI9341_resetHard(Timer_t timer) {
    /**
     *  The LCD driver's RESET pin requires a negative logic (i.e. active `LOW`)
     *  signal for >= 10 [us] and an additional 5 [ms] before further commands
     *  can be sent.
     */
    Timer_setMode(timer, ONESHOT, UP);

    SPI_CLEAR_RESET();
    Timer_Wait1ms(timer, 1);
    SPI_SET_RESET();
    Timer_Wait1ms(timer, 5);
    return;
}

void ILI9341_resetSoft(Timer_t timer) {
    Timer_setMode(timer, ONESHOT, UP);

    SPI_WriteCmd(SWRESET);
    Timer_Wait1ms(timer, 5);               /// the driver needs 5 [ms] before another command
    return;
}

/******************************************************************************
Configuration
*******************************************************************************/

static void ILI9341_setMode(uint8_t param) {
    /**
     * @fn      ILI9341_setMode()
     *
     *          This function simply groups each of the configuration functions
     *          into one to reduce code duplication.
     */

    switch(param) {
        case(SLEEP_ON):
        case(SLEEP_OFF):
            SPI_WriteCmd(param);
            ili9341.sleepMode = param;
            break;
        case(NORMAL_AREA):
        case(PARTIAL_AREA):
            SPI_WriteCmd(param);
            ili9341.displayArea = param;
            break;
        case(FULL_COLORS):
        case(PARTIAL_COLORS):
            SPI_WriteCmd(param);
            ili9341.colorExpression = param;
            break;
        case(INVERT_OFF):
        case(INVERT_ON):
            SPI_WriteCmd(param);
            ili9341.invertMode = param;
            break;
        case(OUTPUT_OFF):
        case(OUTPUT_ON):
            SPI_WriteCmd(param);
            ili9341.outputMode = param;
            break;
        case(COLORDEPTH_16BIT):
        case(COLORDEPTH_18BIT):
            SPI_WriteCmd(PIXSET);
            SPI_WriteData(param);
            break;
        default:
            Assert(false);
            break;
    }

    return;
}

void ILI9341_setSleepMode(sleepMode_t sleepMode, Timer_t timer) {
    /**
     *      The MCU must wait >= 5 [ms] before sending further commands
     *      regardless of the selected mode.
     *
     *      It's also necessary to wait 120 [ms] before
     *      sending `SPLOUT` after sending `SPLIN` or a reset,
     *      so this function waits 120 [ms] regardless of the preceding event.
     */
    Assert(ili9341.isInit);
    ILI9341_setMode(sleepMode);

    Timer_setMode(timer, ONESHOT, UP);
    Timer_Wait1ms(timer, 120);

    return;
}

void ILI9341_setDisplayArea(displayArea_t displayArea) {
    Assert(ili9341.isInit);
    ILI9341_setMode(displayArea);

    return;
}

void ILI9341_setColorExpression(colorExpr_t colorExpr) {
    Assert(ili9341.isInit);
    ILI9341_setMode(colorExpr);

    return;
}

void ILI9341_setPartialArea(uint16_t rowStart, uint16_t rowEnd) {
    // ensure `rowStart` and `rowEnd` meet restrictions.
    rowEnd = (rowEnd > 0) ? rowEnd : 1;
    rowEnd = (rowEnd < ILI9341_NUM_ROWS) ? rowEnd : (ILI9341_NUM_ROWS - 1);
    rowStart = (rowStart > 0) ? rowStart : 1;
    rowStart = (rowStart < rowEnd) ? rowStart : rowEnd;

    // configure and send command sequence
    FIFO_Put(ILI9341_Fifo, ((rowStart & 0xFF00) >> 8));
    FIFO_Put(ILI9341_Fifo, (rowStart & 0x00FF));
    FIFO_Put(ILI9341_Fifo, ((rowEnd & 0xFF00) >> 8));
    FIFO_Put(ILI9341_Fifo, (rowEnd & 0x00FF));
    ILI9341_sendParams(PLTAR);

    return;
}

void ILI9341_setDispInversion(invertMode_t invertMode) {
    Assert(ili9341.isInit);
    ILI9341_setMode(invertMode);

    return;
}

bool ILI9341_isDispInverted(void) {
    return (ili9341.invertMode == INVERT_ON) ? true : false;
}

void ILI9341_setDispOutput(outputMode_t outputMode) {
    /// TODO: Write description
    Assert(ili9341.isInit);
    ILI9341_setMode(outputMode);

    return;
}

bool ILI9341_isOutputOn(void) {
    return (ili9341.outputMode == OUTPUT_ON) ? true : false;
}

void ILI9341_setMemAccessCtrl(bool areRowsFlipped, bool areColsFlipped, bool areRowsColsSwitched,
                              bool isVertRefreshFlipped, bool isColorOrderFlipped,
                              bool isHorRefreshFlipped) {
    /**
     *   This function implements the "Memory Access Control" (`MADCTL`) command
     * from p. 127-128 of the ILI9341 datasheet, which controls how the LCD driver
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

    uint8_t param = 0x00;
    param = (areRowsFlipped) ? (param | 0x80) : param;
    param = (areColsFlipped) ? (param | 0x40) : param;
    param = (areRowsColsSwitched) ? (param | 0x20) : param;
    param = (isVertRefreshFlipped) ? (param | 0x10) : param;
    param = (isColorOrderFlipped) ? (param | 0x08) : param;
    param = (isHorRefreshFlipped) ? (param | 0x04) : param;

    SPI_WriteCmd(MADCTL);
    SPI_WriteData(param);
    return;
}

void ILI9341_setColorDepth(colorDepth_t colorDepth) {
    Assert(ili9341.isInit);
    ILI9341_setMode(colorDepth);

    return;
}

bool ILI9341_isColorDepth16bit(void) {
    return (ili9341.colorDepth == COLORDEPTH_16BIT) ? true : false;
}

void ILI9341_setFrameRate(uint8_t divisionRatio, uint8_t clocksPerLine) {
    /// TODO: Write description

    Cmd_t cmd;
    if(ili9341.colorExpression == PARTIAL_COLORS) {
        cmd = FRMCTR2;
    }
    else {
        cmd = (ili9341.displayArea == NORMAL_AREA) ? FRMCTR1 : FRMCTR3;
    }

    SPI_WriteCmd((uint8_t) cmd);
    SPI_WriteData(divisionRatio & 0x03);
    SPI_WriteData(clocksPerLine & 0x1F);
    return;
}

// void ILI9341_setBlankingPorch(uint8_t vpf, uint8_t vbp, uint8_t hfp, uint8_t
// hbp) {
//     /// TODO: Write
// }

void ILI9341_setInterface(void) {
    /**
     *   This function implements the "Interface Control" `IFCTL` command from
     *   p. 192-194 of the ILI9341 datasheet, which controls how the LCD driver
     *   handles 16-bit data and what interfaces (internal or external) are used.
     *
     *   Name     | Bit # | Param # | Effect when set = `1`
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
     *   bits are cleared and/or irrelevant since the RGB and VSYNC interfaces
     *   aren't used.
     */

    SPI_WriteData(IFCTL);
    SPI_WriteData(0);
    SPI_WriteData(0);
    SPI_WriteData(0);
    return;
}

/******************************************************************************
Memory Writing
*******************************************************************************/

inline static void ILI9341_sendParams(Cmd_t cmd) {
    if(cmd != NOP) {
        SPI_WriteCmd(cmd);
    }

    uint8_t numParams = FIFO_getCurrSize(ILI9341_Fifo);
    while(numParams > 0) {
        uint8_t data = FIFO_Get(ILI9341_Fifo);
        SPI_WriteData(data);

        numParams -= 1;
    }

    return;
}

inline static void ILI9341_setAddress(uint16_t startAddress, uint16_t endAddress, bool is_row) {
    /**
    This function implements the "Column Address Set" (`CASET`) and "Page
    Address Set" (`PASET`) commands from p. 110-113 of the ILI9341 datasheet.

    The input parameters represent the first and last addresses to be written
    to when `ILI9341_writePixel()` is called.

    To work correctly, `startAddress` must be no greater than `endAddress`,
    and `endAddress` cannot be greater than the max number of rows/columns.
    */

    uint8_t cmd = (is_row) ? PASET : CASET;
    uint16_t max_num = (is_row) ? ILI9341_NUM_ROWS : ILI9341_NUM_COLS;

    // ensure `startAddress` and `endAddress` meet restrictions
    Assert(endAddress < max_num);
    Assert(startAddress <= endAddress);

    // configure and send command sequence
    FIFO_Put(ILI9341_Fifo, ((startAddress & 0xFF00) >> 8));
    FIFO_Put(ILI9341_Fifo, (startAddress & 0x00FF));
    FIFO_Put(ILI9341_Fifo, ((endAddress & 0xFF00) >> 8));
    FIFO_Put(ILI9341_Fifo, (endAddress & 0x00FF));

    ILI9341_sendParams(cmd);

    return;
}

void ILI9341_setRowAddress(uint16_t startRow, uint16_t endRow) {
    /**
        This function is simply an interface to ILI9341_setAddress().
        To work correctly, `start_row` must be no greater than `end_row`, and
        `end_row` cannot be greater than the max row number (default 320).
    */
    ILI9341_setAddress(startRow, endRow, true);
    return;
}

void ILI9341_setColAddress(uint16_t startCol, uint16_t endCol) {
    /**
        This function is simply an interface to ILI9341_setAddress().
        To work correctly, `start_col` must be no greater than `end_col`, and
        `end_col` cannot be greater than the max column number (default 240).
    */
    ILI9341_setAddress(startCol, endCol, false);
    return;
}

void ILI9341_writeMemCmd(void) {
    SPI_WriteCmd(RAMWR);
    return;
}

void ILI9341_writePixel(uint8_t red, uint8_t green, uint8_t blue, bool is_16bit) {
    // clang-format off
    /**
     *  This function sends one pixel to the display. Because the serial interface
     *  (SPI) is used, each pixel requires 2 transfers in 16-bit mode and 3
     *  transfers in 18-bit mode. 
     * 
     *  The following table (adapted from p. 63 of the datasheet) visualizes 
     *  how the RGB data is sent to the display when using 16-bit color depth.
     *  
     *  Transfer |                    1              ||||||||                         2                    ||||||||     
     *  ---------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----
     *  Bit #    |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0
     *  Value    | R4  | R3  | R2  | R1  | R0  | G5  | G4  | G3  | G2  | G1  | G0  | B4  | B3  | B2  | B1  | B0
     * 
     *  The following table (adapted from p. 64 of the datasheet) visualizes 
     *  how the RGB data is sent to the display when using 18-bit color depth.
     * 
     *  Transfer |                    1              ||||||||       2        |||
     *  ---------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----
     *  Bit #    |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |  7  |  6  | ...
     *  Value    |  R5 | R4  | R3  | R2  | R1  | R0  | 0/1 | 0/1 |  G5 |  G4 | ...
     */
    // clang-format on

    if(is_16bit) {
        FIFO_Put(ILI9341_Fifo, ((red & 0x1F) << 3) | ((green & 0x38) >> 3));
        FIFO_Put(ILI9341_Fifo, ((green & 0x07) << 5) | (blue & 0x1F));
    }
    else {
        // bits 1 and 0 are set to prevent the TM4C from
        // attempting to right-justify the RGB data
        FIFO_Put(ILI9341_Fifo, ((red & 0x3F) << 2) + 0x03);
        FIFO_Put(ILI9341_Fifo, ((green & 0x3F) << 2) + 0x03);
        FIFO_Put(ILI9341_Fifo, ((blue & 0x3F) << 2) + 0x03);
    }

    ILI9341_sendParams(NOP);

    return;
}

/** @} */
