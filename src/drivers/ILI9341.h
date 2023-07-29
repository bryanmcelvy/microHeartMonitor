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

#include "tm4c123gh6pm.h"

#include "SPI.h"
#include "Timer.h"

/**********************************************************************
Defines
***********************************************************************/

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
// #define RGBSET                  (uint8_t) 0x2D          /// Color Set
#define RAMRD                   (uint8_t) 0x2E          /// Memory Read
#define DISPOFF                 (uint8_t) 0x28          /// Display OFF
#define DISPON                  (uint8_t) 0x29          /// Display ON
#define VSCRDEF                 (uint8_t) 0x33          /// Vertical Scrolling Definition
#define MADCTL                  (uint8_t) 0x36          /// Memory Access Control
#define VSCRSADD                (uint8_t) 0x37          /// Vertical Scrolling Start Address
#define PIXSET                  (uint8_t) 0x3A          /// Pixel Format Set
// #define WRITE_MEMORY_CONTINUE   (uint8_t) 0x3C          /// Write_Memory_Continue
// #define READ_MEMORY_CONTINUE    (uint8_t) 0x3E          /// Read_Memory_Continue
#define WRDISBV                 (uint8_t) 0x51          /// Write Display Brightness
#define RDDISBV                 (uint8_t) 0x52          /// Read Display Brightness
#define IFMODE                  (uint8_t) 0xB0          /// RGB Interface Signal Control (i.e. Interface Mode Control)
#define FRMCTR1                 (uint8_t) 0xB1          /// Frame Control Set (Normal Mode)
// #define INVTR                   (uint8_t) 0xB4          /// Display Inversion Control
#define PRCTR                   (uint8_t) 0xB5          /// Blanking Porch Control
#define IFCTL                   (uint8_t) 0xF6          /// Interface Control

// Other useful constants
#define NUM_COLS                (uint16_t) 240
#define NUM_ROWS                (uint16_t) 320

/**********************************************************************
SECTIONS
***********************************************************************

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

/**********************************************************************
Initialization/Reset
***********************************************************************/

/**
 * @brief       Initialize the LCD driver.
 * 
 */
void ILI9341_Init(void);

/**
 * @brief       Perform a hardware reset of the LCD driver.
 * 
 */
void ILI9341_ResetHard(void);

/**
 * @brief       Perform a software reset of the LCD driver.
 * 
 */
void ILI9341_ResetSoft(void);

/**********************************************************************
Reading Display Status
***********************************************************************/

uint8_t * ILI9341_getDispStatus(void);

uint8_t ILI9341_getMemAccessCtrl(void);

//TODO getPixelFormat

/******************************************************************************
Memory Reading/Writing
*******************************************************************************/

/**
 * @brief       Sets the start/end rows to be written to.
 * 
 * @param       start_row:      0 <= `start_row` <= `end_row`
 * @param       end_row:        `start_row` <= `end_row` < 320
 */
void ILI9341_setRowAddress(uint16_t start_row, uint16_t end_row);

/**
 * @brief
 *      Sets the start/end rows to be written to.
 *      Should be called along with `ILI9341_setRowAddress()` and before 
        `ILI9341_writeMemCmd()`.
 * 
 * @param       start_col:      0 <= `start_col` <= `end_col`
 * @param       end_col:        `start_col` <= `end_col` < 240
 */
void ILI9341_setColAddress(uint16_t start_col, uint16_t end_col);

/**
 * @brief
 *      Sends the "Write Memory" (`RAMWR`) command to the LCD driver,
 *      signalling that incoming data should be written to memory. Should be
 *      be called after setting the row (`ILI9341_setRowAddress`) a
 *      and/or column (`ILI9341_setRowAddress`) addresses, but before
 *      writing image data (`ILI9341_write1px()`).
 */
void ILI9341_writeMemCmd(void);

/**
 * @brief       Write a single pixel to memory.
 *              Should be called after `ILI9341_writeMemCmd()`.
 * @param data 
 */
void ILI9341_write1px(uint8_t red, uint8_t green, uint8_t blue);

//TODO: readMem

/**********************************************************************
Display Config.
***********************************************************************/

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
void ILI9341_setDispInversion(uint8_t is_ON);

/**
 * @brief       Send command to turn the display ON or OFF.
 * 
 * @param       is_ON `1` to turn ON, `0` to turn OFF
 */
void ILI9341_setDisplayStatus(uint8_t is_ON);

void ILI9341_setVertScrollArea(
                                uint16_t top_fixed, 
                                uint16_t vert_scroll,
                                uint16_t bottom_fixed
        );

void ILI9341_setVertScrollStart(uint16_t start_address);

void ILI9341_setMemAccessCtrl(  
        uint8_t row_address_order, uint8_t col_address_order,
        uint8_t row_col_exchange, uint8_t vert_refresh_order,
        uint8_t rgb_order, uint8_t hor_refresh_order
        );

void ILI9341_setPixelFormat(uint8_t is_16bit);

/**
 * @brief       Sets the brightness value of the display.
 * 
 * @param       brightness value between `0` (lowest) and `255` (highest)
 */
void ILI9341_setDispBrightness(uint8_t brightness);

uint8_t ILI9341_getDispBrightness(void);

/**********************************************************************
Other
***********************************************************************/

/**
 * @brief       
 *      Send the "No Operation" command (`NOP`) to the LCD driver.
 *      Can be used to terminate the "Memory Write" (`RAMWR`
 *      "Memory Read" (`RAMRD`) commands, but does nothing otherwise.
 */
void ILI9341_NoOpCmd(void);

// NOTE: The RGB interface is not usable via SPI, so this function was useless.
// void ILI9341_setRGBInterface(uint8_t param);

void ILI9341_setFrameRate(uint8_t div_ratio, uint8_t clocks_per_line);

void ILI9341_setBlankingPorch(uint8_t vert_front_porch, uint8_t vert_back_porch,
                                uint8_t hor_front_porch, uint8_t hor_back_porch);

/**
 * @brief       Sets the interface for the ILI9341.
 * 
 */
void ILI9341_setInterface(void);

#endif

/** @} */
