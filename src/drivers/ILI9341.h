/**
 * @addtogroup  lcd
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

#ifndef __ILI9341_H__
#define __ILI9341_H__

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

#include "tm4c123gh6pm.h"

#include <stdint.h>
#include <stdbool.h>

// Defines
#define NUM_COLS                (uint16_t) 240
#define NUM_ROWS                (uint16_t) 320

/******************************************************************************
Initialization/Reset
*******************************************************************************/

/// @brief      Initialize the LCD driver, the SPI module, and Timer2A.
void ILI9341_Init(void);

/// @brief      Perform a hardware reset of the LCD driver.
void ILI9341_ResetHard(void);

/// @brief      Perform a software reset of the LCD driver.
void ILI9341_ResetSoft(void);

/******************************************************************************
Configuration
*******************************************************************************/

/**
 * @brief       Enter or exit sleep mode. The LCD driver is in sleep mode by
 *              default upon powering on or either kind of reset.
 * 
 * @param       is_sleeping `true` to enter sleep mode, `false` to exit
 */
void ILI9341_setSleepMode(bool is_sleeping);

/**
 * @brief       Set the display to normal mode or partial mode. The LCD driver
 *              starts out in normal mode. Calling with either possible value
 *              exits scrolling mode.
 * 
 * @param       is_normal 'true' for normal mode, 'false' for partial mode
 */
void ILI9341_setDisplayMode(bool is_normal);

/**
 * @brief       Set the partial display area for partial mode.
 *              Call before activating partial mode via ILI9341_setDisplayMode().
 * 
 * @param       rowStart 
 * @param       rowEnd 
 */
void ILI9341_setPartialArea(uint16_t rowStart, uint16_t rowEnd);

/**
 * @brief       Send command to toggle display display inversion
 * 
 * @param       is_ON `1` to turn ON, `0` to turn OFF
 */
void ILI9341_setDispInversion(bool is_ON);

/**
 * @brief       Send command to turn the display ON or OFF.
 * 
 * @param       is_ON `1` to turn ON, `0` to turn OFF
 */
void ILI9341_setDisplayStatus(bool is_ON);

///TODO: Write
void ILI9341_setVertScrollArea(
                                uint16_t top_fixed, 
                                uint16_t vert_scroll,
                                uint16_t bottom_fixed
        );

///TODO: Write
void ILI9341_setVertScrollStart(uint16_t start_address);

/**
 * @brief
 *      Set how data is converted from memory to display.
 * 
 * @param areRowsFlipped 
 * @param areColsFlipped 
 * @param areRowsColsSwitched 
 * @param isVertRefreshFlipped 
 * @param isColorOrderFlipped 
 * @param isHorRefreshFlipped 
 */
void ILI9341_setMemAccessCtrl(  bool areRowsFlipped, bool areColsFlipped,
                                bool areRowsColsSwitched, bool isVertRefreshFlipped,
                                bool isColorOrderFlipped, bool isHorRefreshFlipped);

/**
 * @brief
 *      Set the pixel format to be 16-bit (65K colors) or 18-bit (262K colors).
 *
 * @param is_16bit 
 */
void ILI9341_setColorDepth(bool is_16bit);

/**
 * @brief       
 *      Send the "No Operation" command (`NOP`) to the LCD driver.
 *      Can be used to terminate the "Memory Write" (`RAMWR`
 *      "Memory Read" (`RAMRD`) commands, but does nothing otherwise.
 */
void ILI9341_NoOpCmd(void);

///TODO: Write
void ILI9341_setFrameRate(uint8_t div_ratio, uint8_t clocks_per_line);

///TODO: Write
void ILI9341_setBlankingPorch(  uint8_t vpf, uint8_t vbp, 
                                uint8_t hfp, uint8_t hbp);

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
 * @param       start_row:      0 <= `start_row` <= `end_row`
 * @param       end_row:        `start_row` <= `end_row` < 320
 */
void ILI9341_setRowAddress(uint16_t start_row, uint16_t end_row);

/**
 * @brief       Sets the start/end rows to be written to.
 * 
 *              Should be called along with `ILI9341_setRowAddress()` and 
 *              before `ILI9341_writeMemCmd()`.
 * 
 * @param       start_col:      0 <= `start_col` <= `end_col`
 * @param       end_col:        `start_col` <= `end_col` < 240
 */
void ILI9341_setColAddress(uint16_t start_col, uint16_t end_col);

/**
 * @brief
 *      Sends the "Write Memory" (`RAMWR`) command to the LCD driver,
 *      signalling that incoming data should be written to memory. 
 * 
 *      Should be called after setting the row (`ILI9341_setRowAddress()`) and/or 
 *      and/or column (`ILI9341_setRowAddress()`) addresses, but before writing
 *      image data (`ILI9341_write1px()`).
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
 */
void ILI9341_write1px(uint8_t red, uint8_t green, uint8_t blue, bool is_16bit);

#endif

/** @} */
