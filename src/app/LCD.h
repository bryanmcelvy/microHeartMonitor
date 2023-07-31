/**
 * @file
 * @author  Bryan McElvy
 * @brief   Module for outputting the ECG waveform and HR to a liquid crystal display (LCD).
 */

#ifndef __LCD_H__
#define __LCD_H__

/******************************************************************************
Includes
*******************************************************************************/
#include "ILI9341.h"

#include "SPI.h"
#include "Timer.h"

#include "tm4c123gh6pm.h"

#include <stdint.h>

/******************************************************************************
Initialization and Configuration
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
 * @brief   Draw a horizontal line onto the display.
 * 
 * @param   rowCenter row that the line will be centered on
 * @param   lineWidth width of the line; needs to be odd
 */
void LCD_drawHLine(uint16_t rowCenter, uint16_t lineWidth);

/**
 * @brief   Draw a vertical line onto the display.
 * 
 * @param   colCenter column that the line will be centered on
 * @param   lineWidth width of the line; needs to be odd
 */
void LCD_drawVLine(uint16_t colCenter, uint16_t lineWidth);

/**
 * @brief   Draw an `N` x `N` square onto the display.
 *          The top-left corner will be located at (`startRow`, `start`Col`).
 * 
 * @param   startRow    index of top-most row; `0 <= startRow < NUM_ROWS`
 * @param   startCol    index of left-most column; `0 <= startRow < NUM_COLS`
 * @param   N           length of each side; `0 < N < NUM_COLS`
 */
void LCD_drawSquare(uint16_t startRow, uint16_t startCol, uint16_t N);

#endif
