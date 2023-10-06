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
 *              ILI9341 LCD driver chip via the SPI (serial peripheral interface)
 *              protocol.
 */

#ifndef ILI9341_H
#define ILI9341_H

/******************************************************************************
SECTIONS
        Preprocessor Directives
        Initialization/Reset
        Configuration
        Memory Writing
*******************************************************************************/

/******************************************************************************
Preprocessor Directives
*******************************************************************************/

// Includes
#include "SPI.h"
#include "Timer.h"

#include "FIFO.h"

#include "tm4c123gh6pm.h"

#include <stdbool.h>
#include <stdint.h>

// Defines
enum { ILI9341_NUM_COLS = 240, ILI9341_NUM_ROWS = 320 };

/******************************************************************************
Initialization/Reset
*******************************************************************************/

/**
 * @brief               Initialize the LCD driver and the SPI module.
 *
 * @param[in] timer     Hardware timer to use during initialization.
 */
void ILI9341_Init(Timer_t timer);

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

enum { ON = true, OFF = false };

/**
 * @brief                       Enter or exit sleep mode.
 *
 * @param[in] isSleeping        `true` to enter sleep mode, `false` to exit
 * @param[in] timer             Hardware timer to use after mode change.
 */
void ILI9341_setSleepMode(bool isSleeping, Timer_t timer);

/**
 * @brief       Set the display area and color expression.
 *
 *              Normal mode is the default and allows output to the full
 *              display area. Partial mode should be activated after calling
 *              `ILI9341_setPartialArea()`.
 *
 *              Setting `isFullColors` to `false` restricts the color expression
 *              to 8 colors, determined by the MSB of the R/G/B values.
 *
 * @param       isNormal        `true` for normal mode, `false` for partial mode
 * @param       isFullColors    `true` for full colors, `false` for 8 colors
 */
void ILI9341_setDispMode(bool isNormal, bool isFullColors);

/**
 * @brief       Set the partial display area for partial mode.
 *              Call before activating partial mode via ILI9341_setDisplayMode().
 *
 * @param       rowStart
 * @param       rowEnd
 */
void ILI9341_setPartialArea(uint16_t rowStart, uint16_t rowEnd);

/**
 * @brief               Toggle display inversion (`OFF` by default).
 *
 * @param[in] is_ON
 *
 * @post                Display colors are either inverted (`ON`) or not inverted (`OFF`).
 */
void ILI9341_setDispInversion(bool is_ON);

/**
 * @brief               Change whether the IC is outputting to the display for not.
 *
 * @param[in] is_ON
 *
 * @post                If `ON`, the IC outputs data from its memory to the display.
 *                      If `OFF`, the display is cleared and the IC stops outputting data.
 */
void ILI9341_setDispOutput(bool is_ON);

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
    ILI9341_COLORDEPTH_16BIT = 0x55,               ///< 16-bit color depth
    ILI9341_COLORDEPTH_18BIT = 0x66,               ///< 18-bit color depth
} colorDepth_t;

/**
 * @brief               Set the pixel format to be 16-bit (65K colors) or 18-bit (262K colors).
 *
 * @param[in] colorDepth
 *
 * @post                `16BIT` mode allows for ~65K (2^16) colors and requires 2 transfers.
 *                      `18BIT` mode allows for ~262K (2^18) colors but requires 3 transfers.
 */
void ILI9341_setColorDepth(colorDepth_t colorDepth);

/// TODO: Write brief
void ILI9341_setFrameRateNorm(uint8_t divisionRatio, uint8_t clocksPerLine);

/// TODO: Write brief
void ILI9341_setFrameRateIdle(uint8_t divisionRatio, uint8_t clocksPerLine);

/**
 * @brief       Sets the interface for the ILI9341.
 *              The parameters for this command are hard-coded, so it only
 *              needs to be called once upon initialization.
 */
void ILI9341_setInterface(void);

/// not using backlight, so these aren't necessary
// void ILI9341_setDispBrightness(uint8_t brightness);
// uint8_t ILI9341_getDispBrightness(void);

// NOTE: The RGB interface is not usable via SPI, so this function was useless.
// void ILI9341_setRGBInterface(uint8_t param);

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
 * @brief               Sets the start/end rows to be written to.
 *
 * @param[in] startCol  0 <= `startCol` <= `endCol`
 * @param[in] endCol    startCol` <= `endCol` < 240
 *
 * @see                 ILI9341_setRowAddress, ILI9341_writePixel()
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
 * @param[in] is_16bit  `true` for 16-bit (65K colors, 2 transfers) color depth,
 *                      `false` for 18-bit (262K colors, 3 transfer) color depth
 *
 * @see                 ILI9341_setColorDepth, ILI9341_writeMemCmd(), ILI9341_writePixel()
 */
void ILI9341_writePixel(uint8_t red, uint8_t green, uint8_t blue, bool is_16bit);

#endif               // ILI9341_H

/** @} */
