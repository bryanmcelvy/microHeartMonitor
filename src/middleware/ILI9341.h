/**
 * @addtogroup  ili9341
 * @{
 *
 * @file
 * @author      Bryan McElvy
 * @brief       Driver module for interfacing with an ILI9341 LCD driver
 *
 * @details     This module contains functions for initializing and outputting
 *              graphical data to a 240RGBx320 resolution, 262K color-depth liquid
 *              crystal display (LCD). The module interfaces the LaunchPad (or any
 *              other board featuring the TM4C123GH6PM microcontroller) with an
 *              ILI9341 LCD driver chip via the serial peripheral interface (SPI)
 *              protocol.
 */

#ifndef ILI9341_H
#define ILI9341_H

/******************************************************************************
SECTIONS
        Declarations
        Initialization/Reset
        Configuration
        Memory Writing
*******************************************************************************/

#include "GPIO.h"
#include "SPI.h"
#include "Timer.h"

#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
Declarations
*******************************************************************************/

enum {
    ILI9341_NUM_COLS = 240,               ///< # of columns available on the display
    ILI9341_NUM_ROWS = 320                ///< # of rows available on the display
};

// Selected commands from the datasheet
typedef enum {
    NOP = 0x00,                    ///< No Operation
    SWRESET = 0x01,                ///< Software Reset
    SPLIN = 0x10,                  ///< Enter Sleep Mode
    SPLOUT = 0x11,                 ///< Sleep Out (i.e. Exit Sleep Mode)
    PTLON = 0x12,                  ///< Partial Display Mode ON
    NORON = 0x13,                  ///< Normal Display Mode ON
    DINVOFF = 0x20,                ///< Display Inversion OFF
    DINVON = 0x21,                 ///< Display Inversion ON
    CASET = 0x2A,                  ///< Column Address Set
    PASET = 0x2B,                  ///< Page Address Set
    RAMWR = 0x2C,                  ///< Memory Write
    DISPOFF = 0x28,                ///< Display OFF
    DISPON = 0x29,                 ///< Display ON
    PLTAR = 0x30,                  ///< Partial Area
    VSCRDEF = 0x33,                ///< Vertical Scrolling Definition
    MADCTL = 0x36,                 ///< Memory Access Control
    VSCRSADD = 0x37,               ///< Vertical Scrolling Start Address
    IDMOFF = 0x38,                 ///< Idle Mode OFF
    IDMON = 0x39,                  ///< Idle Mode ON
    PIXSET = 0x3A,                 ///< Pixel Format Set
    FRMCTR1 = 0xB1,                ///< Frame Rate Control Set (Normal Mode)
    FRMCTR2 = 0xB2,                ///< Frame Rate Control Set (Idle Mode)
    FRMCTR3 = 0xB3,                ///< Frame Rate Control Set (Partial Mode)
    PRCTR = 0xB5,                  ///< Blanking Porch Control
    IFCTL = 0xF6,                  ///< Interface Control
} Cmd_t;

// clang-format off
/* Currently unused commands
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
Initialization/Reset
*******************************************************************************/

/**
 * @brief                   Initialize the LCD driver.
 *
 * @pre                     Initialize the GPIO port.
 * @pre                     Initialize the SPI module.
 * @pre                     Initialize the Timer.
 *
 * @param[in] resetPinPort  The GPIO port that the `RESET` pin belongs to.
 * @param[in] resetPin      The GPIO pin used as the `RESET` pin.
 * @param[in] spi           The SPI module to use for communication.
 * @param[in] timer         The hardware timer to use during initialization.
 *
 * @post                    The `RESET` is configured as a digital `OUTPUT` pin.
 * @post                    The SPI is configured and enabled.
 * @post                    The LCD driver is initialized and ready to accept commands.
 *
 * @see                     GPIO_InitPort(), SPI_Init(), Timer_Init()
 */
void ILI9341_Init(GpioPort_t resetPinPort, GpioPin_t resetPin, Spi_t spi, Timer_t timer);

/**
 * @brief               Sets the interface for the ILI9341.
 *
 *                      The parameters for this command are hard-coded, so it only
 *                      needs to be called once upon initialization.
 */
void ILI9341_setInterface(void);

/**
 * @brief               Perform a hardware reset of the LCD driver.
 *
 * @param[in] timer     Hardware timer to use during reset.
 */
void ILI9341_resetHard(Timer_t timer);

/**
 * @brief               Perform a software reset of the LCD driver.
 *
 * @param[in] timer     Hardware timer to use during reset.
 */
void ILI9341_resetSoft(Timer_t timer);

/******************************************************************************
Configuration
*******************************************************************************/

typedef enum {
    SLEEP_ON = SPLIN,
    SLEEP_OFF = SPLOUT
} sleepMode_t;

/**
 * @brief                       Enter or exit sleep mode (`ON` by default).
 *
 * @param[in] sleepMode         `SLEEP_ON` or `SLEEP_OFF`
 * @param[in] timer             Hardware timer to use for a slight delay after the mode change.
 *
 * @post                        The IC will be in or out of sleep mode depending
 *                              on the value of `sleepMode`.
 */
void ILI9341_setSleepMode(sleepMode_t sleepMode, Timer_t timer);

typedef enum {
    NORMAL_AREA = NORON,
    PARTIAL_AREA = PTLON
} displayArea_t;

/**
 * @brief                   Set the display area.
 *
 * @pre                     If using partial mode, set the partial area first.
 *
 * @param[in] displayArea   `NORMAL_AREA` or `PARTIAL_AREA`
 *
 * @see                     ILI9341_setPartialArea()
 */
void ILI9341_setDisplayArea(displayArea_t displayArea);

/**
 * @brief                   Set the display area for partial mode.
 *                          Call before activating partial mode.
 *
 * @param[in] rowStart
 * @param[in] rowEnd
 *
 * @see                     ILI9341_setDisplayArea()
 */
void ILI9341_setPartialArea(uint16_t rowStart, uint16_t rowEnd);

typedef enum {
    FULL_COLORS = IDMOFF,
    PARTIAL_COLORS = IDMON
} colorExpr_t;

/**
 * @brief                   Set the color expression (`FULL_COLORS` by default).
 *
 * @param[in] colorExpr     `FULL_COLORS` or `PARTIAL_COLORS`
 *
 * @post                    With partial color expression, the display only uses 8 colors.
 *                          Otherwise, the color depth determines the number of colors available.
 */
void ILI9341_setColorExpression(colorExpr_t colorExpr);

typedef enum {
    INVERT_ON = DINVON,
    INVERT_OFF = DINVOFF
} invertMode_t;

/**
 * @brief                   Toggle display inversion (`OFF` by default).
 *
 * @param[in] invertMode    `INVERT_ON` or `INVERT_OFF`
 *
 * @post                    When inversion is ON, the display colors are inverted.
 *                          (e.g. BLACK -> WHITE, GREEN -> PURPLE)
 */
void ILI9341_setDispInversion(invertMode_t invertMode);

typedef enum {
    OUTPUT_ON = DISPON,
    OUTPUT_OFF = DISPOFF
} outputMode_t;

/**
 * @brief                   Change whether the IC is outputting to the display for not.
 *
 * @param[in] outputMode    `OUTPUT_ON` or `OUTPUT_OFF`
 *
 * @post                    If `ON`, the IC outputs data from its memory to the display.
 *                          If `OFF`, the display is cleared and the IC stops outputting data.
 */
void ILI9341_setDispOutput(outputMode_t outputMode);

/**
 * @brief                               Set how data is converted from memory to display.
 *
 * @param[in] areRowsFlipped
 * @param[in] areColsFlipped
 * @param[in] areRowsAndColsSwitched
 * @param[in] isVertRefreshFlipped
 * @param[in] isColorOrderFlipped
 * @param[in] isHorRefreshFlipped
 */
void ILI9341_setMemAccessCtrl(bool areRowsFlipped, bool areColsFlipped, bool areRowsAndColsSwitched,
                              bool isVertRefreshFlipped, bool isColorOrderFlipped,
                              bool isHorRefreshFlipped);

typedef enum {
    COLORDEPTH_16BIT = 0x55,
    COLORDEPTH_18BIT = 0x66,
} colorDepth_t;

/**
 * @brief                   Set the color depth for the display.
 *
 * @param[in] colorDepth    `COLORDEPTH_16BIT` or `COLORDEPTH_18BIT`
 *
 * @post                    `16BIT` mode allows for ~65K (2^16) colors and requires 2 transfers.
 *                          `18BIT` mode allows for ~262K (2^18) colors but requires 3 transfers.
 */
void ILI9341_setColorDepth(colorDepth_t colorDepth);

/// TODO: Write brief
void ILI9341_setFrameRate(uint8_t divisionRatio, uint8_t clocksPerLine);

/* NOTE: not using backlight, so these aren't necessary at the moment
void ILI9341_setDispBrightness(uint8_t brightness);
uint8_t ILI9341_getDispBrightness(void);
*/

/* NOTE: The RGB interface is not usable via SPI, so this function was useless.
void ILI9341_setRGBInterface(uint8_t param);
*/

/******************************************************************************
Memory Writing
*******************************************************************************/

/**
 * @brief               Sets the start/end rows to be written to.
 *
 * @param[in] startRow  0 <= `startRow` <= `endRow`
 * @param[in] endRow    startRow` <= `endRow` < 240
 *
 * @see                 ILI9341_setRowAddress, ILI9341_writePixel()
 */
void ILI9341_setRowAddress(uint16_t startRow, uint16_t endRow);

/**
 * @brief               Sets the start/end columns to be written to.
 *
 * @param[in] startCol  0 <= `startCol` <= `endCol`
 * @param[in] endCol    startCol` <= `endCol` < 240
 *
 * @see                 ILI9341_setColAddress, ILI9341_writePixel()
 */
void ILI9341_setColAddress(uint16_t startCol, uint16_t endCol);

/**
 * @brief               Signal to the driver that pixel data is incoming and
 *                      should be written to memory.
 *
 * @pre                 Set the row and/or column addresses.
 * @post                The LCD driver is ready to accept pixel data.
 *
 * @see                 ILI9341_setRowAddress, ILI9341_setColAddress(), ILI9341_writePixel()
 */
void ILI9341_writeMemCmd(void);

/**
 * @brief               Write a single pixel to frame memory.
 *
 * @pre                 Send the "Write Memory" command.
 * @pre                 Set the desired color depth for the display.
 *
 * @param[in] red       5 or 6-bit `R` value
 * @param[in] green     5 or 6-bit `G` value
 * @param[in] blue      5 or 6-bit `B` value
 *
 * @see                 ILI9341_setColorDepth, ILI9341_writeMemCmd(), ILI9341_writePixel()
 */
void ILI9341_writePixel(uint8_t red, uint8_t green, uint8_t blue);

#endif               // ILI9341_H

/** @} */
