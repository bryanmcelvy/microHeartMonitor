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
#include <stdint.h>

// Selected commands from the datasheet
#define NOP                     (uint8_t) 0x00          /// No Operation
#define SWRESET                 (uint8_t) 0x01          /// Software Reset

#define RDDST                   (uint8_t) 0x09          /// Read Display Status
#define RDDMADCTL               (uint8_t) 0x0B          /// Read Display MADCTL
#define RDDCOLMOD               (uint8_t) 0x0C          /// Read Display Pixel Format

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

/*      SECTIONS
        Initialization/Reset
        Reading Display Status
        Memory Reading/Writing
        Display Config.
                Inversion
                ON/OFF
                Scrolling
                Memory Access Control
                Pixel Format
                Brightness
        Other
                RGB Interface
                Frame Rate Control
                Blanking Porch Control

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

/**********************************************************************
Memory Reading/Writing
***********************************************************************/

void ILI9341_setRowAddress(uint8_t start_row, uint8_t end_row);
void ILI9341_setColAddress(uint8_t start_col, uint8_t end_col);
void ILI9341_write(uint8_t data[]);
//TODO: readMem

/**********************************************************************
Display Config.
***********************************************************************/

/**
 * @brief       Send command to turn the display ON or OFF.
 * 
 * @param       is_ON `1` to turn ON, `0` to turn OFF
 */
void ILI9341_setDispInv(uint8_t is_ON);
void ILI9341_setDisplay(uint8_t is_ON);
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
void ILI9341_setDispBrightness(uint8_t brightness);
uint8_t ILI9341_getDispBrightness(void);

/**********************************************************************
Other
***********************************************************************/

/**
 * @brief       Send command to set operation status of display interface
 * 
 * @param       param 
 */
void ILI9341_setDispInterface(uint8_t param);
void ILI9341_setFrameRate(uint8_t div_ratio, uint8_t clocks_per_line);
void ILI9341_setBlankingPorch(uint8_t vert_front_porch, uint8_t vert_back_porch,
                                uint8_t hor_front_porch, uint8_t hor_back_porch);

#endif

/** @} */
