/**
 * @addtogroup lcd
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Module for outputting the ECG waveform and HR to a liquid crystal display (LCD).
 */

#ifndef LCD_H
#define LCD_H

/******************************************************************************
SECTIONS
        Preprocessor Directives
        Initialization/Configuration
        Drawing Area
        Color
        Drawing
        Scrolling
*******************************************************************************/

/******************************************************************************
Preprocessor Directives
*******************************************************************************/

// Dependencies
#include "ILI9341.h"

#include "SPI.h"
#include "Timer.h"

#include "tm4c123gh6pm.h"

#include <stdint.h>
#include <stdbool.h>

// Dimensions
#define X_MAX          NUM_ROWS
#define Y_MAX          NUM_COLS

// 3-bit Color Codes
#define LCD_BLACK      (uint8_t) 0x00

#define LCD_RED        (uint8_t) 0x04
#define LCD_GREEN      (uint8_t) 0x02
#define LCD_BLUE       (uint8_t) 0x01

#define LCD_YELLOW     (uint8_t) 0x06               // LCD_RED + LCD_GREEN
#define LCD_CYAN       (uint8_t) 0x03               // LCD_BLUE + LCD_GREEN
#define LCD_PURPLE     (uint8_t) 0x05               // LCD_RED + LCD_BLUE
#define LCD_WHITE      (uint8_t) 0x07               // LCD_RED + LCD_BLUE + LCD_GREEN

// 3-bit Color Codes (Inverted)
#define LCD_BLACK_INV  (uint8_t) LCD_WHITE                // LCD_WHITE - LCD_BLACK

#define LCD_RED_INV    (uint8_t) LCD_CYAN                 // LCD_WHITE - LCD_RED
#define LCD_GREEN_INV  (uint8_t) LCD_PURPLE               // LCD_WHITE - LCD_GREEN
#define LCD_BLUE_INV   (uint8_t) LCD_YELLOW               // LCD_WHITE - LCD_BLUE

#define LCD_YELLOW_INV (uint8_t) LCD_BLUE                 // LCD_WHITE - LCD_YELLOW
#define LCD_CYAN_INV   (uint8_t) LCD_RED                  // LCD_WHITE - LCD_CYAN
#define LCD_PURPLE_INV (uint8_t) LCD_GREEN                // LCD_WHITE - LCD_PURPLE
#define LCD_WHITE_INV  (uint8_t) LCD_BLACK                // LCD_WHITE - LCD_WHITE

/******************************************************************************
Initialization/Configuration
*******************************************************************************/
/** @name Init./Config. Functions */               /// @{

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

///@}

/******************************************************************************
Drawing Area
*******************************************************************************/
/** @name Drawing Area Definition Functions */               /// @{

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
void LCD_setArea(uint16_t x1_new, uint16_t x2_new, uint16_t y1_new, uint16_t y2_new);

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

///@}

/******************************************************************************
Color
*******************************************************************************/
/** @name Color Setting Functions */               /// @{

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

///@}

/******************************************************************************
Drawing
*******************************************************************************/
/** @name Drawing Functions */               /// @{

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
void LCD_drawRectangle(uint16_t x1, uint16_t dx, uint16_t y1, uint16_t dy, bool is_filled);

/******************************************************************************
Scrolling
*******************************************************************************/

/**
 * @brief               Draw a rectangle of size `dx` x `dy` and blank out all
 *                      other pixels between `y_min` and `y_max`.
 *
 * @param x1            lowest (left-most) x-coordinate
 * @param dx            length (horizontal distance) of the column
 * @param y1            y-coordinate of the pixel's bottom side
 * @param dy            height (vertical distance) of the pixel
 * @param y_min         lowest (bottom-most) y-coordinate
 * @param y_max         highest (top-most) y-coordinate
 * @param color_code    3-bit color code
 */
void LCD_graphSample(uint16_t x1, uint16_t dx, uint16_t y1, uint16_t dy, uint16_t y_min,
                     uint16_t y_max, uint16_t color_code);

///@}

/** @} */

#endif               // LCD_H
