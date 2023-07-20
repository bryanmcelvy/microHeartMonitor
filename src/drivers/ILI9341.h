/**
 * @addtogroup  lcd
 * @{
 * 
 * @file
 * @author      Bryan McElvy
 * @brief       Driver module for interfacing with an ILI9341 LCD driver
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

#include "SPI.h"
#include "Timer.h"

void ILI9341_Init(void);
void ILI9341_ResetHard(void);
void ILI9341_ResetSoft(void);
void ILI9341_DisplayOn(void);
void ILI9341_DisplayOff(void);
void ILI9341_SetFrameRate(uint8_t frame_rate);
/*  TODO
        –Memory Write
        –Page Address Set (row(s) select)
        –Column Address Set (column(s) select)
*/

#endif

/** @} */
