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

//**TODO: Add commands + hex codes from datasheet**

#ifndef __ILI9341_H__
#define __ILI9341_H__

#include "tm4c123gh6pm.h"

#include "SPI.h"
#include "Timer.h"

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
