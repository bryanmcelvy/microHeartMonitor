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
#define NUM_COLS (uint16_t) 240
#define NUM_ROWS (uint16_t) 320

/******************************************************************************
Initialization/Reset
*******************************************************************************/

/// @brief      Initialize the LCD driver, the SPI module, and Timer2A.
void ILI9341_Init(void);

/// @brief      Perform a hardware reset of the LCD driver.
void ILI9341_resetHard(void);

/// @brief      Perform a software reset of the LCD driver.
void ILI9341_resetSoft(void);

/******************************************************************************
Configuration
*******************************************************************************/

/**
 * @brief       Enter or exit sleep mode. The LCD driver is in sleep mode by
 *              default upon powering on or either kind of reset.
 *
 * @param       isSleeping `true` to enter sleep mode, `false` to exit
 */
void ILI9341_setSleepMode(bool isSleeping);

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
 * @brief       Toggle display inversion. Turning `ON` causes colors to be
 *              inverted on the display.
 *
 * @param       is_ON `true` to turn ON, `false` to turn OFF
 */
void ILI9341_setDispInversion(bool is_ON);

/**
 * @brief       Turn display output ON or OFF. This function clears the display
 *              and stops outputting to the display area, but does not affect
 *              frame memory or power.
 *
 * @param       is_ON `true` to turn ON, `false` to turn OFF
 */
void ILI9341_setDispOutput(bool is_ON);

/**
 * @brief                       Set the vertical scrolling area of the display.
 *                              The sum of the three parameters should be equal
 *                              to the max number of rows `NUM_ROWS = 320`.
 *
 * @param topFixedArea          Number of rows fixed at the top of the screen.
 * @param vertScrollArea        Number of rows that scroll.
 * @param bottFixedArea         Number of rows fixed at the bottom of the screen.
 */
void ILI9341_setScrollArea(uint16_t topFixedArea, uint16_t vertScrollArea, uint16_t bottFixedArea);

/**
 * @brief               Set the start row for vertical scrolling.
 *
 * @param startRow      Start row for scrolling. Should be `>= topFixedArea - 1`
 */
void ILI9341_setScrollStart(uint16_t startRow);

// TODO: Finish writing description
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

/**
 * @brief Set the pixel format to be 16-bit (65K colors) or 18-bit (262K colors).
 *
 * @param is_16bit
 */
void ILI9341_setColorDepth(bool is_16bit);

/**
 * @brief       Send the "No Operation" command (`NOP` = `0x00`) to the LCD driver.
 *              Can be used to terminate the "Memory Write" (`RAMWR`) and
 *              "Memory Read" (`RAMRD`) commands, but does nothing otherwise.
 */
void ILI9341_NoOpCmd(void);

/// TODO: Write brief
void ILI9341_setFrameRateNorm(uint8_t divisionRatio, uint8_t clocksPerLine);

/// TODO: Write brief
void ILI9341_setFrameRateIdle(uint8_t divisionRatio, uint8_t clocksPerLine);

/// TODO: Write
void ILI9341_setBlankingPorch(uint8_t vpf, uint8_t vbp, uint8_t hfp, uint8_t hbp);

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
 * @brief       Sets the start/end rows to be written to.
 *
 *              Should be called along with `ILI9341_setColAddress()` and
 *              before `ILI9341_writeMemCmd()`.
 *
 * @param       startRow:      0 <= `startRow` <= `endRow`
 * @param       endRow:        `startRow` <= `endRow` < 320
 */
void ILI9341_setRowAddress(uint16_t startRow, uint16_t endRow);

/**
 * @brief       Sets the start/end rows to be written to.
 *
 *              Should be called along with `ILI9341_setRowAddress()` and
 *              before `ILI9341_writeMemCmd()`.
 *
 * @param       startCol:      0 <= `startCol` <= `endCol`
 * @param       endCol:        `startCol` <= `endCol` < 240
 */
void ILI9341_setColAddress(uint16_t startCol, uint16_t endCol);

/**
 * @brief
 *      Sends the "Write Memory" (`RAMWR`) command to the LCD driver,
 *      signalling that incoming data should be written to memory.
 *
 *      Should be called after setting the row (`ILI9341_setRowAddress()`) and/or
 *      and/or column (`ILI9341_setRowAddress()`) addresses, but before writing
 *      image data (`ILI9341_writePixel()`).
 */
void ILI9341_writeMemCmd(void);

/**
 * @brief               Write a single pixel to frame memory.
 *
 *                      Call `ILI9341_writeMemCmd()` before this one.
 *
 * @param red           5 or 6-bit `R` value
 * @param green         5 or 6-bit `G` value
 * @param blue          5 or 6-bit `B` value
 * @param is_16bit      `true` for 16-bit (65K colors, 2 transfers) color depth,
 *                      `false` for 18-bit (262K colors, 3 transfer) color depth
 *                      NOTE: set color depth via `ILI9341_setColorDepth()`
 */
void ILI9341_writePixel(uint8_t red, uint8_t green, uint8_t blue, bool is_16bit);

#endif               // ILI9341_H

/** @} */
