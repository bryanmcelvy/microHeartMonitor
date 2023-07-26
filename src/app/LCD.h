/**
 * @file
 * @author  Bryan McElvy
 * @brief   Module for outputting the ECG waveform and HR to a liquid crystal display (LCD).
 */

#ifndef __LCD_H__
#define __LCD_H__

#include "ILI9341.h"

/**
 * @brief   Initializes and configures the ILI9341 LCD driver.
 * 
 */
void LCD_Init(void);

#endif
