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
        Plotting Parameters
        Drawing
*******************************************************************************/

#include "ILI9341.h"

#include <stdbool.h>

/******************************************************************************
Initialization
*******************************************************************************/
/** @name Initialization & Configuration */               /// @{

/**
 * @brief               Initialize the LCD.
 *
 * @post                The display will be ready to accept commands, but output will be off.
 */
void LCD_Init(void);

/**
 * @brief               Toggle display output `ON` or `OFF` (`OFF` by default).
 *
 * @param[in] isOn      `true` to turn display output `ON`, `false` to turn `OFF`
 *
 * @post                When `OFF`, the display is cleared.
 *                      When `ON`, the IC writes pixel data from its memory to the display.
 */
void LCD_setOutputMode(bool isOn);

/******************************************************************************
Plotting Parameters
*******************************************************************************/

enum LCD_PLOT_INFO {
    LCD_X_MAX = ILI9341_NUM_ROWS - 1,
    LCD_Y_MAX = ILI9341_NUM_COLS - 1
};

/**
 * @brief               Set new x-coordinates to be written to.
 *                      \f$ 0 <= x1 <= x2 <= X_{MAX} \f$
 *
 * @param[in] x1        left-most x-coordinate
 * @param[in] x2        right-most x-coordinate
 *
 * @see                 LCD_setY()
 */
void LCD_setX(uint16_t x1, uint16_t x2);

/**
 * @brief               Set new y-coordinates to be written to.
 *                      \f$ 0 <= y1 <= y2 <= Y_{MAX} \f$
 *
 * @param[in] y1        lowest y-coordinate
 * @param[in] y2        highest y-coordinate
 *
 * @see                 LCD_setX()
 */
void LCD_setY(uint16_t y1, uint16_t y2);

enum LCD_COLORS {
    // Bits 2, 1, 0 correspond to R, G, and B values, respectively.
    // NOTE: since the display colors are inverted, these bit patterns are too.
    LCD_BLACK = 0x00 ^ 0x07,

    LCD_RED = 0x04 ^ 0x07,
    LCD_GREEN = 0x02 ^ 0x07,
    LCD_BLUE = 0x01 ^ 0x07,

    LCD_YELLOW = 0x06 ^ 0x07,
    LCD_CYAN = 0x03 ^ 0x07,
    LCD_PURPLE = 0x05 ^ 0x07,
    LCD_WHITE = 0x07 ^ 0x07
};

/**
 * @brief               Set the color value.
 *
 * @param[in]           color Color to use.
 *
 * @post                Outgoing pixel data will use the selected color.
 */
void LCD_setColor(uint8_t color);

/** @} */               // Initialization & Configuration

/******************************************************************************
Drawing
*******************************************************************************/
/** @name Drawing */               /// @{

/**
 * @brief               Draw on the LCD.
 *
 * @pre                 Set the drawable area and the color to use for that area.
 *
 * @post                The selected areas of the display will be drawn onto with
 *                      the selected color.
 *
 * @see                 LCD_setX(), LCD_setY(), LCD_setColor()
 * @showrefs
 */
void LCD_Draw(void);

/**
 * @brief               Fill the display with a single color.
 *
 * @pre                 Select the desired color to fill the display with.
 *
 * @see                 LCD_setColor()
 */
void LCD_Fill(void);

/**
 * @brief               Draw a horizontal line across the entire display.
 *
 * @pre                 Select the desired color to use for the line.
 *
 * @param[in] yCenter   y-coordinate to center the line on
 * @param[in] lineWidth width of the line; should be a positive, odd number
 *
 * @see                 LCD_drawVertLine, LCD_drawRectangle()
 */
void LCD_drawHoriLine(uint16_t yCenter, uint16_t lineWidth);

/**
 * @brief               Draw a vertical line across the entire display.
 *
 * @pre                 Select the desired color to use for the line.
 *
 * @param[in] xCenter   x-coordinate to center the line on
 * @param[in] lineWidth width of the line; should be a positive, odd number
 *
 * @see                 LCD_drawHoriLine, LCD_drawRectangle()
 */
void LCD_drawVertLine(uint16_t xCenter, uint16_t lineWidth);

/**
 * @brief               Draw a rectangle of size `dx` x `dy` onto the display.
 *                      The bottom-left corner will be located at `(x1, y1)`.
 *
 * @pre                 Select the desired color to use for the rectangle.
 *
 * @param[in] x1        lowest (left-most) x-coordinate
 * @param[in] dx        length (horizontal distance) of the rectangle
 * @param[in] y1        lowest (bottom-most) y-coordinate
 * @param[in] dy        height (vertical distance) of the rectangle
 *
 * @see                 LCD_Draw(), LCD_Fill(), LCD_drawHoriLine(), LCD_drawVertLine()
 */
void LCD_drawRectangle(uint16_t x1, uint16_t dx, uint16_t y1, uint16_t dy);

inline static void LCD_plotSample(uint16_t x, uint16_t y, uint8_t color) {
    LCD_setColor(color);
    LCD_setX(x, x);
    LCD_setY(y, y);
    LCD_Draw();
    return;
}

/** @} */               // Drawing

/******************************************************************************
Writing
*******************************************************************************/
/** @name Writing */               /// @{

enum LCD_WRITING_INFO {
    HEIGHT_CHAR = 8,
    LEN_CHAR = 5,

    NUM_LINES = 30,
    NUM_COLS = 64
};

/**
 * @brief               Set the cursor to line `x`, column `y`.
 *
 * @param[in] lineNum   Line number to place characters. Should be in range `[0, 30)`.
 * @param[in] colNum    Column number to place characters. Should be in range `[0, 64)`.
 */
void LCD_setCursor(uint16_t lineNum, uint16_t colNum);

// TODO: Write description
void LCD_writeChar(unsigned char inputChar);

// TODO: Write description
void LCD_writeStr(void * asciiString);

// TODO: Write description
void LCD_writeInt(int32_t num);

// TODO: Write description
void LCD_writeFloat(float num);

/// @} // Writing

/** @} */               // lcd

#endif                  // LCD_H
