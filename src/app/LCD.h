/**
 * @addtogroup lcd
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Header file for LCD module.
 *
 *          This module is essentially a higher-level interface to the ILI9341 module.
 */

#ifndef LCD_H
#define LCD_H

/******************************************************************************
SECTIONS
        Initialization
        Configuration
        Drawing Area
        Color
        Drawing
        Scrolling
*******************************************************************************/

#include "ILI9341.h"

#include "SPI.h"
#include "Timer.h"

#include "NewAssert.h"

#include "tm4c123gh6pm.h"

#include <stdbool.h>
#include <stdint.h>

enum {
    LCD_X_MAX = ILI9341_NUM_ROWS,
    LCD_Y_MAX = ILI9341_NUM_COLS
};

/******************************************************************************
Initialization
*******************************************************************************/
/** @name Init./Config. Functions */               /// @{

/**
 * @brief           Initialize the LCD.
 *
 * @post            The display will be ready to accept commands, but output will be off.
 */
void LCD_Init(void);

/******************************************************************************
Configuration
*******************************************************************************/

/**
 * @brief           Toggle display output `ON` or `OFF` (`OFF` by default).
 *                  Turning output `OFF` stops the LCD driver chip from writing
 *                  to the display, and also blanks out the display completely.
 *
 * @param[in] isOn  `true` to turn display output `ON`, `false` to turn `OFF`
 *
 * @see             LCD_toggleOutput()
 */
void LCD_setOutputMode(bool isOn);

/// @}

/******************************************************************************
Drawing Area
*******************************************************************************/

/**
 * @brief               Set new x-coordinates to be written to.
 *                      \f$ 0 <= x1 <= x2 < X_MAX \f$
 *
 * @param x1_new        left-most x-coordinate
 * @param x2_new        right-most x-coordinate
 *
 * @see                 LCD_setY()
 */
void LCD_setX(uint16_t x1_new, uint16_t x2_new);

/**
 * @brief               Set new y-coordinates to be written to.
 *                      \f$ 0 <= y1 <= y2 < Y_MAX \f$
 *
 * @param y1_new        lowest y-coordinate
 * @param y2_new        highest y-coordinate
 *
 * @see                 LCD_setX()
 */
void LCD_setY(uint16_t y1_new, uint16_t y2_new);

/******************************************************************************
Color
*******************************************************************************/
/** @name Color Setting Functions */               /// @{

typedef enum {
    // Bits 2, 1, 0 correspond to R, G, and B values, respectively.
    // 3-bit Color Codes
    LCD_BLACK = 0x00,
    LCD_RED = 0x04,
    LCD_GREEN = 0x02,
    LCD_BLUE = 0x01,
    LCD_YELLOW = 0x06,               // LCD_RED + LCD_GREEN
    LCD_CYAN = 0x03,                 // LCD_BLUE + LCD_GREEN
    LCD_PURPLE = 0x05,               // LCD_RED + LCD_BLUE
    LCD_WHITE = 0x07,                // LCD_RED + LCD_BLUE + LCD_GREEN

    // 3-bit Color Codes (Inverted)
    LCD_BLACK_INV = LCD_WHITE,                // LCD_WHITE - LCD_BLACK

    LCD_RED_INV = LCD_CYAN,                   // LCD_WHITE - LCD_RED
    LCD_GREEN_INV = LCD_PURPLE,               // LCD_WHITE - LCD_GREEN
    LCD_BLUE_INV = LCD_YELLOW,                // LCD_WHITE - LCD_BLUE

    LCD_YELLOW_INV = LCD_BLUE,                // LCD_WHITE - LCD_YELLOW
    LCD_CYAN_INV = LCD_RED,                   // LCD_WHITE - LCD_CYAN
    LCD_PURPLE_INV = LCD_GREEN,               // LCD_WHITE - LCD_PURPLE
    LCD_WHITE_INV = LCD_BLACK                 // LCD_WHITE - LCD_WHITE
} LCD_Color_t;

/**
 * @brief               Set the color value via a 3-bit code.
 *
 * @param color         Color to use.
 *
 * @see                 LCD_setColorDepth(), LCD_toggleColorDepth(), LCD_setColor()
 */
void LCD_setColor(LCD_Color_t color);

/// @}

/******************************************************************************
Drawing
*******************************************************************************/
/** @name Drawing Functions */               /// @{

/**
 * @brief               Draw on the LCD.
 *
 * @pre                 Set the drawable area and the color to use for that area.
 * @post                The selected areas of the display will be drawn onto with
 *                      the selected color.
 *
 * @see                 LCD_setX(), LCD_setY()
 */
void LCD_Draw(void);

/**
 * @brief               Fill the display with a single color.
 */
void LCD_Fill(void);

/**
 * @brief               Draw a horizontal line across the entire display.
 *
 * @param yCenter       y-coordinate to center the line on
 * @param lineWidth     width of the line; should be a positive, odd number
 *
 * @see                 LCD_drawVertLine, LCD_drawRectangle()
 */
void LCD_drawHoriLine(uint16_t yCenter, uint16_t lineWidth);

/**
 * @brief               Draw a vertical line across the entire display.
 *
 * @param xCenter       x-coordinate to center the line on
 * @param lineWidth     width of the line; should be a positive, odd number
 *
 * @see                 LCD_drawHoriLine, LCD_drawRectangle()
 */
void LCD_drawVertLine(uint16_t xCenter, uint16_t lineWidth);

/**
 * @brief               Draw a rectangle of size `dx` x `dy` onto the display.
 *                      The bottom-left corner will be located at `(x1, y1)`.
 *
 * @param x1            lowest (left-most) x-coordinate
 * @param dx            length (horizontal distance) of the rectangle
 * @param y1            lowest (bottom-most) y-coordinate
 * @param dy            height (vertical distance) of the rectangle
 * @param isFilled      `true` to fill the rectangle, `false` to leave it unfilled
 */
void LCD_drawRectangle(uint16_t x1, uint16_t dx, uint16_t y1, uint16_t dy, bool isFilled);

/// @}

/** @} */               // lcd

#endif                  // LCD_H
