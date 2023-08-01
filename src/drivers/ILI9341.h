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

#include "SPI.h"
#include "Timer.h"

#include "tm4c123gh6pm.h"

#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
Defines
*******************************************************************************/

// Selected commands from the datasheet
#define NOP                     (uint8_t) 0x00          /// No Operation
#define SWRESET                 (uint8_t) 0x01          /// Software Reset
#define RDDST                   (uint8_t) 0x09          /// Read Display Status
#define RDDMADCTL               (uint8_t) 0x0B          /// Read Display MADCTL
#define RDDCOLMOD               (uint8_t) 0x0C          /// Read Display Pixel Format
#define SPLIN                   (uint8_t) 0x10          /// Enter Sleep Mode
#define SPLOUT                  (uint8_t) 0x11          /// Sleep Out (i.e. Exit Sleep Mode)
#define DINVOFF                 (uint8_t) 0x20          /// Display Inversion OFF
#define DINVON                  (uint8_t) 0x21          /// Display Inversion ON
#define CASET                   (uint8_t) 0x2A          /// Column Address Set
#define PASET                   (uint8_t) 0x2B          /// Page Address Set
#define RAMWR                   (uint8_t) 0x2C          /// Memory Write
#define RAMRD                   (uint8_t) 0x2E          /// Memory Read
#define DISPOFF                 (uint8_t) 0x28          /// Display OFF
#define DISPON                  (uint8_t) 0x29          /// Display ON
#define VSCRDEF                 (uint8_t) 0x33          /// Vertical Scrolling Definition
#define MADCTL                  (uint8_t) 0x36          /// Memory Access Control
#define VSCRSADD                (uint8_t) 0x37          /// Vertical Scrolling Start Address
#define PIXSET                  (uint8_t) 0x3A          /// Pixel Format Set
#define FRMCTR1                 (uint8_t) 0xB1          /// Frame Control Set (Normal Mode)
#define PRCTR                   (uint8_t) 0xB5          /// Blanking Porch Control
#define IFCTL                   (uint8_t) 0xF6          /// Interface Control

// Other useful constants
#define NUM_COLS                (uint16_t) 240
#define NUM_ROWS                (uint16_t) 320

/* NOTE: not being used at the moment
// #define RGBSET                  (uint8_t) 0x2D          /// Color Set
// #define WRITE_MEMORY_CONTINUE   (uint8_t) 0x3C          /// Write_Memory_Continue
// #define READ_MEMORY_CONTINUE    (uint8_t) 0x3E          /// Read_Memory_Continue
// #define WRDISBV                 (uint8_t) 0x51          /// Write Display Brightness
// #define RDDISBV                 (uint8_t) 0x52          /// Read Display Brightness
// #define IFMODE                  (uint8_t) 0xB0          /// RGB Interface Signal Control (i.e. Interface Mode Control)
// #define INVTR                   (uint8_t) 0xB4          /// Display Inversion Control
*/

/******************************************************************************
SECTIONS
*******************************************************************************

Initialization/Reset
Reading Display Status
Memory Reading/Writing
Display Config.
        -Inversion
        -ON/OFF
        -Scrolling
        -Memory Access Control
        -Pixel Format
        -Brightness
Other
        -RGB Interface
        -Frame Rate Control
        -Blanking Porch Control
        -Interface Control
*/

/******************************************************************************
Initialization/Reset
*******************************************************************************/

/// @brief      Initialize the LCD driver.
void ILI9341_Init(void);

/// @brief      Perform a hardware reset of the LCD driver.
void ILI9341_ResetHard(void);

/// @brief      Perform a software reset of the LCD driver.
void ILI9341_ResetSoft(void);

/******************************************************************************
Reading Display Status
*******************************************************************************/

///TODO: Write
uint8_t * ILI9341_getDispStatus(void);

///TODO: Write
uint8_t ILI9341_getMemAccessCtrl(void);

//TODO getPixelFormat

/******************************************************************************
Memory Reading/Writing
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

//TODO: readMem

/******************************************************************************
Display Config.
*******************************************************************************/

/**
 * @brief       Send command to enter or exit sleep mode
 * 
 * @param       is_sleeping `1` to enter sleep mode, `0` to exit
 */
void ILI9341_sleepMode(uint8_t is_sleeping);

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
 * @brief       Send command to set the LCD driver's read/write scanning direction.
 * 
 * @param row_order             `0` for up-to-down, `1` for down-to-up
 * @param col_order             `0` for left-to-right, `1` for right-to-left
 * @param row_col_exchange      `0` for default, `1` to switch invert rows/columns
 * @param vert_refresh_order    `0` for top-to-bottom, `1` for bottom-to-top
 * @param rgb_order             TODO: figure out what this actually does
 * @param hor_refresh_order     `0` for left-to-right, `1` for right-to-left
 */
void ILI9341_setMemAccessCtrl(  bool row_order, bool col_order,
                                bool row_col_exchange, bool vert_refresh_order,
                                bool rgb_order, bool hor_refresh_order);

/**
 * @brief
 *      Set the pixel format to be 16-bit (65K colors) or 18-bit (262K colors).
 *
 * @param is_16bit 
 */
void ILI9341_setColorDepth(bool is_16bit);

/// not using backlight, so these aren't necessary
// void ILI9341_setDispBrightness(uint8_t brightness);
// uint8_t ILI9341_getDispBrightness(void);

/******************************************************************************
Other
*******************************************************************************/

/**
 * @brief       
 *      Send the "No Operation" command (`NOP`) to the LCD driver.
 *      Can be used to terminate the "Memory Write" (`RAMWR`
 *      "Memory Read" (`RAMRD`) commands, but does nothing otherwise.
 */
void ILI9341_NoOpCmd(void);

// NOTE: The RGB interface is not usable via SPI, so this function was useless.
// void ILI9341_setRGBInterface(uint8_t param);

///TODO: Write
void ILI9341_setFrameRate(uint8_t div_ratio, uint8_t clocks_per_line);

///TODO: Write
void ILI9341_setBlankingPorch(  uint8_t vpf, uint8_t vbp, 
                                uint8_t hfp, uint8_t hbp);

/**
 * @brief       Sets the interface for the ILI9341.
 * 
 */
void ILI9341_setInterface(void);

#endif

/** @} */
