/**
 * @file
 * @author  Bryan McElvy
 * @brief   Module for outputting the ECG waveform and HR to a liquid crystal display (LCD).
 */

#ifndef __LCD_H__
#define __LCD_H__

/******************************************************************************
SECTIONS
        1) Preprocessor Directives
        2) Initialization/Configuration
        3) Drawing Area
        4) Color
        5) Drawing
        6) Scrolling
*******************************************************************************/

/******************************************************************************
1) Preprocessor Directives
*******************************************************************************/

// Dependencies
#include "ILI9341.h"

#include "SPI.h"
#include "Timer.h"

#include "tm4c123gh6pm.h"

#include <stdint.h>
#include <stdbool.h>

// Dimensions
#define X_MAX           NUM_ROWS
#define Y_MAX           NUM_COLS

// 3-bit Color Codes
#define LCD_BLACK       (uint8_t) 0x00

#define LCD_RED         (uint8_t) 0x04
#define LCD_GREEN       (uint8_t) 0x02
#define LCD_BLUE        (uint8_t) 0x01

#define LCD_YELLOW      (uint8_t) 0x06 // LCD_RED + LCD_GREEN
#define LCD_CYAN        (uint8_t) 0x03 // LCD_BLUE + LCD_GREEN
#define LCD_PURPLE      (uint8_t) 0x05 // LCD_RED + LCD_BLUE
#define LCD_WHITE       (uint8_t) 0x07 // LCD_RED + LCD_BLUE + LCD_GREEN

/******************************************************************************
2) Initialization/Configuration
*******************************************************************************/

/**
 * @brief       Initialize the LCD driver and its internal independencies.
 * 
 */
void LCD_Init(void);

/**
 * @brief       Toggle display output `ON` or `OFF` (`OFF` by default).
 *              Turning output `OFF` prevents the LCD driver from refreshing the
 *              display, which can prevent abnormalities like screen tearing
 *              while attempting to update the image.
 */
void LCD_toggleOutput(void);

/// @brief      Toggle color inversion `ON` or `OFF` (`OFF` by default).
void LCD_toggleInversion(void);

/// @brief      Toggle 16-bit or 18-bit color depth (16-bit by default).
void LCD_toggleColorDepth(void);

/******************************************************************************
3) Drawing Area
*******************************************************************************/

/**
 * @brief               Set the area of the display to be written to.
 *                      `0 <= x1 <= x2 < X_MAX`
 *                      `0 <= y1 <= y2 < Y_MAX`
 * 
 * @param x1_new        left-most x-coordinate
 * @param x2_new        right-most x-coordinate
 * @param y1_new        lowest y-coordinate
 * @param y2_new        highest y-coordinate
 */
void LCD_setArea(       uint16_t x1_new, uint16_t x2_new,
                        uint16_t y1_new, uint16_t y2_new);

/**
 * @brief               Set only new x-coordinates to be written to.
 *                      `0 <= x1 <= x2 < X_MAX`
 * 
 * @param x1_new        left-most x-coordinate
 * @param x2_new        right-most x-coordinate
 */
void LCD_setX(uint16_t x1_new, uint16_t x2_new);

/**
 * @brief               Set only new y-coordinates to be written to.
 *                      `0 <= y1 <= y2 < Y_MAX`
 * 
 * @param y1_new        lowest y-coordinate
 * @param y2_new        highest y-coordinate
 */
void LCD_setY(uint16_t y1_new, uint16_t y2_new);

/******************************************************************************
4) Color
*******************************************************************************/

/**
 * @brief               Set the current color value for the display.
 *                      Only the first 5-6 bits of each inputted value are used.
 * 
 * @param   R_val       5-bit (`[0-31]`) R value;
                        6-bit (`[0-63]`) if color depth is 18-bit
 * @param   G_val       6-bit (`[0-63]`) G value
 * @param   B_val       5-bit (`[0-31]`) B value;
                        6-bit (`[0-63]`) if color depth is 18-bit
 */
void LCD_setColor(uint8_t R_val, uint8_t G_val, uint8_t B_val);

/**
 * @brief               Set the color value via a 3-bit code.
 * 
 * @param color_code    3-bit color value to use. Bits 2, 1, 0 correspond to
 *                      R, G, and B values, respectively.
 */
void LCD_setColor_3bit(uint8_t color_code);

/******************************************************************************
5) Drawing
*******************************************************************************/

/**
 * @brief               Draw on the LCD display. 
 *                      Call this function after setting the drawable area via 
 *                      `LCD_setArea()`, or after individually calling
 *                      `LCD_setX()` and/or `LCD_setY()`.
 */
void LCD_draw(void);

/** 
 * @brief               Fill the display with a single color.
 */
void LCD_fill(void);

/**
 * @brief               Draw a horizontal line across the entire display.
 * 
 * @param yCenter       y-coordinate to center the line on        
 * @param lineWidth     width of the line; should be a positive, odd number
 */
void LCD_drawHLine(uint16_t yCenter, uint16_t lineWidth);

/**
 * @brief               Draw a vertical line across the entire display.
 * 
 * @param xCenter       x-coordinate to center the line on        
 * @param lineWidth     width of the line; should be a positive, odd number
 */
void LCD_drawVLine(uint16_t xCenter, uint16_t lineWidth);

/**
 * @brief               Draw a rectangle of size `dx` x `dy` onto the display.
 *                      The bottom-left corner will be located at `(x1, y1)`.
 * 
 * @param x1            lowest (left-most) x-coordinate
 * @param dx            length (horizontal distance) of the rectangle
 * @param y1            lowest (bottom-most) y-coordinate
 * @param dy            height (vertical distance) of the rectangle
 * @param is_filled     `true` to fill the rectangle, `false` to leave it unfilled
 */
void LCD_drawRectangle( uint16_t x1, uint16_t dx,
                        uint16_t y1, uint16_t dy,
                        bool is_filled);

/******************************************************************************
6) Scrolling
*******************************************************************************/

#endif
