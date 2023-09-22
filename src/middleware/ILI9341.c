/**
 * @addtogroup  ili9341
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for ILI9341 module.
 */

/******************************************************************************
TODO
    – Refactor writing functions to use more generic SPI interface
    – Add assertions
*******************************************************************************/

/******************************************************************************
SECTIONS
        Preprocessor Directives
        Static Declarations
        Initialization/Reset
        Configuration
        Memory Writing
*******************************************************************************/

/******************************************************************************
Preprocessor Directives
*******************************************************************************/

// Includes
#include "ILI9341.h"

#include "SPI.h"
#include "Timer.h"

#include "FIFO.h"

#include "tm4c123gh6pm.h"

#include <stdint.h>
#include <stdbool.h>

// Selected commands from the datasheet
// NOTE: NUM_COLS and NUM_ROWS are defined in the header file
typedef enum {
    NOP = 0x00,                    /// No Operation
    SWRESET = 0x01,                /// Software Reset
    SPLIN = 0x10,                  /// Enter Sleep Mode
    SPLOUT = 0x11,                 /// Sleep Out (i.e. Exit Sleep Mode)
    PTLON = 0x12,                  /// Partial Display Mode ON
    NORON = 0x13,                  /// Normal Display Mode ON
    DINVOFF = 0x20,                /// Display Inversion OFF
    DINVON = 0x21,                 /// Display Inversion ON
    CASET = 0x2A,                  /// Column Address Set
    PASET = 0x2B,                  /// Page Address Set
    RAMWR = 0x2C,                  /// Memory Write
    DISPOFF = 0x28,                /// Display OFF
    DISPON = 0x29,                 /// Display ON
    PLTAR = 0x30,                  /// Partial Area
    VSCRDEF = 0x33,                /// Vertical Scrolling Definition
    MADCTL = 0x36,                 /// Memory Access Control
    VSCRSADD = 0x37,               /// Vertical Scrolling Start Address
    IDMOFF = 0x38,                 /// Idle Mode OFF
    IDMON = 0x39,                  /// Idle Mode ON
    PIXSET = 0x3A,                 /// Pixel Format Set
    FRMCTR1 = 0xB1,                /// Frame Rate Control Set (Normal Mode)
    FRMCTR2 = 0xB2,                /// Frame Rate Control Set (Idle Mode)
    FRMCTR3 = 0xB3,                /// Frame Rate Control Set (Partial Mode)
    PRCTR = 0xB5,                  /// Blanking Porch Control
    IFCTL = 0xF6,                  /// Interface Control
} Cmd_t;

// clang-format off
/** Currently unused commands
#define RDDST                   (uint8_t) 0x09          /// Read Display Status
#define RDDMADCTL               (uint8_t) 0x0B          /// Read Display MADCTL
#define RDDCOLMOD               (uint8_t) 0x0C          /// Read Display Pixel Format
#define RGBSET                  (uint8_t) 0x2D          /// Color Set
#define RAMRD                   (uint8_t) 0x2E          /// Memory Read
#define WRITE_MEMORY_CONTINUE   (uint8_t) 0x3C          /// Write_Memory_Continue
#define READ_MEMORY_CONTINUE    (uint8_t) 0x3E          /// Read_Memory_Continue
#define WRDISBV                 (uint8_t) 0x51          /// Write Display Brightness
#define RDDISBV                 (uint8_t) 0x52          /// Read Display Brightness
#define IFMODE                  (uint8_t) 0xB0          /// RGB Interface Signal Control (i.e. Interface Mode Control)
#define INVTR                   (uint8_t) 0xB4          /// Display Inversion Control
 */
// clang-format on

/******************************************************************************
Static Declarations
*******************************************************************************/

static uint32_t ILI9341_Buffer[8];
static FIFO_t * ILI9341_Fifo;

inline static void ILI9341_setAddress(uint16_t start_address, uint16_t end_address, bool is_row);

/**
 * @brief           Send a command and/or the data within the FIFO buffer.
 *                  A command is only sent when `cmd != NOP` (where `NOP = 0`).
 *                  Data is only sent if the FIFO buffer is not empty.
 *
 * @param[in] cmd   Command to send.
 */
inline static void ILI9341_sendParams(Cmd_t cmd);

/******************************************************************************
Initialization/Reset
*******************************************************************************/

void ILI9341_Init(void) {
    SPI_Init();
    Timer2A_Init();

    ILI9341_Fifo = FIFO_Init(ILI9341_Buffer, 8);

    ILI9341_resetHard();
    ILI9341_setInterface();
    return;
}

void ILI9341_resetHard(void) {
    /**
     *  The LCD driver's RESET pin requires a negative logic (i.e. active `LOW`)
     *  signal for >= 10 [us] and an additional 5 [ms] before further commands
     *  can be sent.
     */
    GPIO_PORTA_DATA_R &= ~(0x80);               // clear PA7 to init. reset
    Timer2A_Wait1ms(1);
    GPIO_PORTA_DATA_R |= 0x80;                  // set PA7 to end reset pulse
    Timer2A_Wait1ms(5);
    return;
}

void ILI9341_resetSoft(void) {
    SPI_WriteCmd(SWRESET);
    Timer2A_Wait1ms(5);                         /// the driver needs 5 [ms] before another command
    return;
}

/******************************************************************************
Configuration
*******************************************************************************/

void ILI9341_setSleepMode(bool is_sleeping) {
    /**     This function turns sleep mode ON or OFF
     *      depending on the value of `is_sleeping`.
     *      Either way, the MCU must wait >= 5 [ms]
     *      before sending further commands.
     *
     *      It's also necessary to wait 120 [ms] before
     *      sending `SPLOUT` after sending `SPLIN` or a reset,
     *      so this function waits 120 [ms] regardless of the preceding event.
     */
    if(is_sleeping) {
        SPI_WriteCmd(SPLIN);
    }
    else {
        SPI_WriteCmd(SPLOUT);
    }

    Timer2A_Wait1ms(120);
}

void ILI9341_setDispMode(bool is_normal, bool is_full_colors) {
    if(is_normal) {
        SPI_WriteCmd(NORON);
    }
    else {
        SPI_WriteCmd(PTLON);
    }               // call after ILI9341_setPartialArea()

    if(is_full_colors) {
        SPI_WriteCmd(IDMON);
    }
    else {
        SPI_WriteCmd(IDMOFF);
    }
}

void ILI9341_setPartialArea(uint16_t rowStart, uint16_t rowEnd) {
    // ensure `rowStart` and `rowEnd` meet restrictions.
    rowEnd = (rowEnd > 0) ? rowEnd : 1;
    rowEnd = (rowEnd < NUM_ROWS) ? rowEnd : (NUM_ROWS - 1);
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

void ILI9341_setDispInversion(bool is_ON) {
    /// TODO: Write description
    if(is_ON) {
        SPI_WriteCmd(DINVON);
    }
    else {
        SPI_WriteCmd(DINVOFF);
    }
}

void ILI9341_setDispOutput(bool is_ON) {
    /// TODO: Write description
    if(is_ON) {
        SPI_WriteCmd(DISPON);
    }
    else {
        SPI_WriteCmd(DISPOFF);
    }
}

void ILI9341_setScrollArea(uint16_t top_fixed, uint16_t vert_scroll, uint16_t bottom_fixed) {

    // ensure parameters sum together
    // while( (top_fixed + vert_scroll + bottom_fixed) < NUM_ROWS ) {
    //     vert_scroll += 1;
    // }
    // while( (top_fixed + vert_scroll + bottom_fixed) > NUM_ROWS ) {
    //     vert_scroll -= 1;
    // }

    // configure and send command sequence
    FIFO_Put(ILI9341_Fifo, ((top_fixed & 0xFF00) >> 8));
    FIFO_Put(ILI9341_Fifo, (top_fixed & 0x00FF));
    FIFO_Put(ILI9341_Fifo, ((vert_scroll & 0xFF00) >> 8));
    FIFO_Put(ILI9341_Fifo, (vert_scroll & 0x00FF));
    FIFO_Put(ILI9341_Fifo, ((bottom_fixed & 0xFF00) >> 8));
    FIFO_Put(ILI9341_Fifo, (bottom_fixed & 0x00FF));

    ILI9341_sendParams(VSCRDEF);

    return;
}

void ILI9341_setScrollStart(uint16_t startRow) {
    FIFO_Put(ILI9341_Fifo, ((startRow & 0xFF00) >> 8));
    FIFO_Put(ILI9341_Fifo, (startRow & 0x00FF));
    ILI9341_sendParams(VSCRSADD);

    return;
}

void ILI9341_setMemAccessCtrl(bool areRowsFlipped, bool areColsFlipped, bool areRowsColsSwitched,
                              bool isVertRefreshFlipped, bool isColorOrderFlipped,
                              bool isHorRefreshFlipped) {
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
    return;
}

void ILI9341_setColorDepth(bool is_16bit) {
    /**
     *  16-bit requires 2 transfers and allows for 65K colors.
     *  18-bit requires 3 transfers and allows for 262K colors.
     */
    uint8_t param = (is_16bit) ? 0x55 : 0x66;
    SPI_WriteCmd(PIXSET);
    SPI_WriteData(param);
    return;
}

void ILI9341_NoOpCmd(void) {
    SPI_WriteCmd(NOP);
    return;
}

void ILI9341_setFrameRateNorm(uint8_t div_ratio, uint8_t clocks_per_line) {
    /// TODO: Write description

    div_ratio &= 0x03;
    clocks_per_line &= 0x1F;

    SPI_WriteCmd(FRMCTR1);
    SPI_WriteData(div_ratio);
    SPI_WriteData(clocks_per_line);
    return;
}

void ILI9341_setFrameRateIdle(uint8_t div_ratio, uint8_t clocks_per_line) {
    /// TODO: Write description

    div_ratio &= 0x03;
    clocks_per_line &= 0x1F;

    SPI_WriteCmd(FRMCTR2);
    SPI_WriteData(div_ratio);
    SPI_WriteData(clocks_per_line);
    return;
}

// void ILI9341_setBlankingPorch(uint8_t vpf, uint8_t vbp, uint8_t hfp, uint8_t hbp) {
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
     *   bits are cleared and/or irrelevant since the RGB and VSYNC interfaces aren't used.
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

inline static void ILI9341_setAddress(uint16_t start_address, uint16_t end_address, bool is_row) {
    /**
    This function implements the "Column Address Set" (`CASET`) and "Page
    Address Set" (`PASET`) commands from p. 110-113 of the ILI9341 datasheet.

    The input parameters represent the first and last addresses to be written
    to when `ILI9341_writePixel()` is called.

    To work correctly, `start_address` must be no greater than `end_address`,
    and `end_address` cannot be greater than the max number of rows/columns.
    */

    uint8_t cmd = (is_row) ? PASET : CASET;
    uint16_t max_num = (is_row) ? NUM_ROWS : NUM_COLS;

    // ensure `start_address` and `end_address` meet restrictions
    end_address = (end_address < max_num) ? end_address : (max_num - 1);
    start_address = (start_address < end_address) ? start_address : end_address;

    // configure and send command sequence
    FIFO_Put(ILI9341_Fifo, ((start_address & 0xFF00) >> 8));
    FIFO_Put(ILI9341_Fifo, (start_address & 0x00FF));
    FIFO_Put(ILI9341_Fifo, ((end_address & 0xFF00) >> 8));
    FIFO_Put(ILI9341_Fifo, (end_address & 0x00FF));

    ILI9341_sendParams(cmd);

    return;
}

void ILI9341_setRowAddress(uint16_t start_row, uint16_t end_row) {
    /**
        This function is simply an interface to ILI9341_setAddress().
        To work correctly, `start_row` must be no greater than `end_row`, and
        `end_row` cannot be greater than the max row number (default 320).
    */
    ILI9341_setAddress(start_row, end_row, true);
    return;
}

void ILI9341_setColAddress(uint16_t start_col, uint16_t end_col) {
    /**
        This function is simply an interface to ILI9341_setAddress().
        To work correctly, `start_col` must be no greater than `end_col`, and
        `end_col` cannot be greater than the max column number (default 240).
    */
    ILI9341_setAddress(start_col, end_col, false);
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
