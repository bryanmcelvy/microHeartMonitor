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

// Selected commands from the datasheet
#define NOP                     (uint8_t) 0x00          /// No Operation
#define SWRESET                 (uint8_t) 0x01          /// Software Reset
#define RDDST                   (uint8_t) 0x09          /// Read Display Status
#define RDDMADCTL               (uint8_t) 0x0B          /// Read Display MADCTL
#define DINVOFF                 (uint8_t) 0x20          /// Display Inversion OFF
#define DINVON                  (uint8_t) 0x21          /// Display Inversion ON
#define CASET                   (uint8_t) 0x2A          /// Column Address Set
#define PASET                   (uint8_t) 0x2B          /// Page Address Set
#define RAMWR                   (uint8_t) 0x2C          /// Memory Write
#define RGBSET                  (uint8_t) 0x2D          /// Color Set
#define RAMRD                   (uint8_t) 0x2E          /// Memory Read
#define DISPOFF                 (uint8_t) 0x28          /// Display OFF
#define DISPON                  (uint8_t) 0x29          /// Display ON
#define VSCRDEF                 (uint8_t) 0x33          /// Vertical Scrolling Definition
#define MADCTL                  (uint8_t) 0x36          /// Memory Access Control
#define VSCRSADD                (uint8_t) 0x37          /// Vertical Scrolling Start Address
#define PIXSET                  (uint8_t) 0x3A          /// Pixel Format Set
#define WRITE_MEMORY_CONTINUE   (uint8_t) 0x3C          /// Write_Memory_Continue
#define READ_MEMORY_CONTINUE    (uint8_t) 0x3E          /// Read_Memory_Continue
#define WRDISBV                 (uint8_t) 0x51          /// Write Display Brightness
#define RDDISBV                 (uint8_t) 0x52          /// Read Display Brightness
#define IFMODE                  (uint8_t) 0xB0          /// RGB Interface Signal Control
#define FRMCTR1                 (uint8_t) 0xB1          /// Frame Control Set (Normal Mode)
#define INVTR                   (uint8_t) 0xB4          /// Display Inversion Control
#define PRCTR                   (uint8_t) 0xB5          /// Blanking Porch Control
#define DISCTRL                 (uint8_t) 0xB6          /// Display Function Control

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

/**
 * @brief       Send command to turn the display ON or OFF.
 * 
 * @param       is_ON `1` to turn ON, `0` to turn OFF
 */
void ILI9341_DisplayOn(uint8_t is_ON);

/**
 * @brief       Set the display's frame rate.
 * 
 * @param       frame_rate num. in the range [61, 119] (exact values in datasheet)
 */
void ILI9341_SetFrameRate(uint8_t frame_rate);

/**
 * @brief       Send command to set operation status of display interface
 * 
 */
void ILI9341_SetDispInterface(void);

/*  TODO
        –Memory Write
        –Page Address Set (row(s) select)
        –Column Address Set (column(s) select)
*/

#endif

/** @} */
