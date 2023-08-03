/**
 * @file
 * @author  Bryan McElvy
 * @brief   Module for outputting the ECG waveform and HR to a liquid crystal display (LCD).
 */

#ifndef __LCD_H__
#define __LCD_H__

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

// Includes
#include "ILI9341.h"

#include "SPI.h"
#include "Timer.h"

#include "tm4c123gh6pm.h"

#include <stdint.h>

// Defines (Colors)
#define LCD_RED         (uint8_t) 0x04
#define LCD_GREEN       (uint8_t) 0x02
#define LCD_BLUE        (uint8_t) 0x01

#define LCD_YELLOW      (LCD_RED + LCD_GREEN)
#define LCD_CYAN        (LCD_BLUE + LCD_GREEN)
#define LCD_PURPLE      (LCD_RED + LCD_BLUE)
#define LCD_WHITE       (LCD_RED + LCD_BLUE + LCD_GREEN)

// Defines (Dimensions)
#define X_MAX           NUM_ROWS
#define Y_MAX           NUM_COLS

/******************************************************************************
Initialization/Configuration
*******************************************************************************/
void LCD_Init(void);

/**
 * @brief   Toggle the display ON or OFF (ON by default)
 */
void LCD_toggleStatus(void);

/**
 * @brief   Toggle display inversion ON or OFF (OFF by default)
 * 
 */
void LCD_toggleInversion(void);

/**
 * @brief   Toggle 16-bit or 18-bit color depth (16-bit by default)
 * 
 */
void LCD_toggleColorDepth(void);

/******************************************************************************
Drawing Area
*******************************************************************************/

/**
 * @brief           Set the area of the display to be written to.
 * 
 * @param rowStart  index of top-most row; `0 <= rowStart <= rowEnd`
 * @param rowEnd    index of bottom-most row; `rowStart <= rowEnd < NUM_ROWS`
 * @param colStart  index of left-most column; `0 <= colStart <= colEnd`
 * @param colEnd    index of right-most column; `colStart <= colEnd < NUM_COLS`
 */
void LCD_setArea(   uint16_t rowStart, uint16_t rowEnd,
                    uint16_t colStart, uint16_t colEnd);

/**
 * @brief           Set only the rows to be written to.
 * 
 * @param rowStart  index of top-most row; `0 <= rowStart <= rowEnd`
 * @param rowEnd    index of bottom-most row; `rowStart <= rowEnd < NUM_ROWS`
 */
void LCD_setRow(uint16_t rowStart, uint16_t rowEnd);

/**
 * @brief           Set only the columns to be written to.
 * 
 * @param colStart  index of left-most column; `0 <= colStart <= colEnd`
 * @param colEnd    index of right-most column; `colStart <= colEnd < NUM_COLS`
 */
void LCD_setCol(uint16_t colStart, uint16_t colEnd);

/******************************************************************************
Color
*******************************************************************************/

/**
 * @brief           Set the current color value for the display.
 *                  Only the first 5-6 bits are used.
 * 
 * @param   R_val   5-bit (0-31) R value; 6-bit (0-63) if color depth is 18-bit
 * @param   G_val   6-bit (0-63) G value
 * @param   B_val   5-bit (0-31) B value; 6-bit (0-63) if color depth is 18-bit
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
Drawing
*******************************************************************************/

/**
 * @brief   Draw on the LCD display. This function should be called after
 *          setting the drawable area via `LCD_setArea()`, or after 
 *          individually setting the row and column areas using `LCD_setRow`
 *          and `LCD_setCol`, respectively.
 */
void LCD_draw(void);

/**
 * @brief           Draw a horizontal line onto the display.
 * 
 * @param yCenter   y-coordinate to center the line on        
 * @param lineWidth width of the line; should be a positive, odd number
 */
void LCD_drawHLine(uint16_t yCenter, uint16_t lineWidth);

/**
 * @brief           Draw a vertical line onto the display.
 * 
 * @param xCenter   x-coordinate to center the line on        
 * @param lineWidth width of the line; should be a positive, odd number
 */
void LCD_drawVLine(uint16_t xCenter, uint16_t lineWidth);

/**
 * @brief           Draw an `l` x `h` rectangle on the display.
 *                  The bottom-left corner will be located at `(x1, y1)`.
 * 
 * @param x1        x-coordinate of bottom-left corner
 * @param y1        y-coordinate of bottom-left corner
 * @param dx        AKA `l`; length (horizontal distance) of the rectangle
 * @param dy        AKA `h`; height (vertical distance) of the rectangle
 * @param is_filled `true` to fill the rectangle, `false` to leave it unfilled
 */
void LCD_drawRectangle( uint16_t x1, uint16_t y1,
                        uint16_t dx, uint16_t dy,
                        bool is_filled);

/******************************************************************************
Scrolling
*******************************************************************************/
// void LCD_setScrollArea

#endif
