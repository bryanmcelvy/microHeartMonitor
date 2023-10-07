/**
 * @addtogroup lcd
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for LCD module.
 */

#include "LCD.h"

/******************************************************************************
SECTIONS
        Static Declarations
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

/******************************************************************************
Static Declarations
*******************************************************************************/

/**
 * @brief               Helper function for drawing straight lines.
 *
 * @param center        Row or column that the line is centered on.
 *                      `center` is increased or decreased if the line to be
 *                      written would have gone out of bounds.
 * @param lineWidth     Width of the line. Should be a positive, odd number.
 * @param is_row        `true` for horizontal line, `false` for vertical line
 */
inline static void LCD_drawLine(uint16_t center, uint16_t lineWidth, bool is_horizontal);

static struct {
    uint16_t x1;                 ///< starting x-value in range [0, x2]
    uint16_t x2;                 ///< ending x-value in range [0, NUM_ROWS)
    uint16_t y1;                 ///< starting y-value in range [0, y2]
    uint16_t y2;                 ///< ending x-value in range [0, NUM_COLS)

    uint8_t R_val;               ///< 5 or 6-bit R value
    uint8_t G_val;               ///< 6-bit G value
    uint8_t B_val;               ///< 5 or 6-bit B value

    bool isInit;                 ///< if `true`, LCD has been initialized
} lcd;

/******************************************************************************
Initialization + Configuration
*******************************************************************************/

void LCD_Init(void) {
    Assert(lcd.isInit == false);

    Timer_t timer2 = Timer_Init(TIMER2);
    ILI9341_Init(timer2);
    ILI9341_setSleepMode(SLEEP_OFF, timer2);

    ILI9341_setMemAccessCtrl(1, 0, 0, 0, 1, 0);

    ILI9341_setColorDepth(COLORDEPTH_16BIT);
    ILI9341_setColorExpression(PARTIAL_COLORS);
    ILI9341_setDisplayArea(NORMAL_AREA);

    // ILI9341_setFrameRate(2, 27);               // frame rate = 35 Hz

    LCD_setX(0, (LCD_X_MAX - 1));
    LCD_setY(0, (LCD_Y_MAX - 1));

    ILI9341_setDispInversion(INVERT_ON);
    LCD_setColor(LCD_BLACK_INV);

    ILI9341_setDispOutput(OUTPUT_OFF);

    lcd.isInit = true;

    return;
}

void LCD_setOutputMode(bool isOn) {
    outputMode_t outputMode = (isOn) ? OUTPUT_ON : OUTPUT_OFF;
    ILI9341_setDispOutput(outputMode);

    return;
}

void LCD_setX(uint16_t x1, uint16_t x2) {
    lcd.x1 = x1;
    lcd.x2 = x2;
    ILI9341_setRowAddress(lcd.x1, lcd.x2);
    return;
}

void LCD_setY(uint16_t y1, uint16_t y2) {
    lcd.y1 = y1;
    lcd.y2 = y2;
    ILI9341_setColAddress(lcd.y1, lcd.y2);
    return;
}

void LCD_setColor(LCD_Color_t color) {
    // clang-format off
    /**
     *  This is simply a convenience function for setting the color using the
     *  enum values defined in the header file. The ones with the `_INV` suffix
     *  should be used when the display colors are inverted.
     *
     *  hex     | binary | macro
     *  --------|--------|------------
     *  0x00    |  000   | LCD_BLACK
     *  0x01    |  001   | LCD_BLUE
     *  0x02    |  010   | LCD_GREEN
     *  0x03    |  011   | LCD_CYAN
     *  0x04    |  100   | LCD_RED
     *  0x05    |  101   | LCD_PURPLE
     *  0x06    |  110   | LCD_YELLOW
     *  0x07    |  111   | LCD_WHITE
     */
    // clang-format on

    if(color == LCD_BLACK) {
        lcd.R_val = 1;
        lcd.G_val = 1;
        lcd.B_val = 1;
    }
    else {
        lcd.R_val = 0x1F * (color & 0x04);
        lcd.B_val = 0x1F * (color & 0x01);
        lcd.G_val = 0x3F * (color & 0x02);
    }

    return;
}

/******************************************************************************
Drawing
*******************************************************************************/

void LCD_Draw(void) {
    /// @showrefs
    uint32_t numPixels = (uint32_t) ((lcd.x2 - lcd.x1) + 1) * ((lcd.y2 - lcd.y1) + 1);

    ILI9341_writeMemCmd();
    for(uint32_t count = 0; count < numPixels; count++) {
        ILI9341_writePixel(lcd.R_val, lcd.G_val, lcd.B_val, true);
    }

    return;
}

void LCD_Fill(void) {
    LCD_setX(0, LCD_X_MAX - 1);
    LCD_setY(0, LCD_Y_MAX - 1);

    LCD_Draw();

    return;
}

inline static void LCD_drawLine(uint16_t center, uint16_t lineWidth, bool is_horizontal) {
    // ensure `lineWidth` is odd and positive
    lineWidth = (lineWidth > 0) ? lineWidth : 1;
    lineWidth = ((lineWidth % 2) == 0) ? lineWidth : (lineWidth - 1);

    uint16_t padding = ((lineWidth - 1) / 2);

    // ensure line does not go out-of-bounds
    uint16_t MAX_NUM = (is_horizontal) ? LCD_Y_MAX : LCD_X_MAX;

    if(center < padding) {
        center = padding + 1;
    }
    else if(center >= (MAX_NUM - padding)) {
        center = (MAX_NUM - padding) - 1;
    }

    // set start and end row/column, and draw
    uint16_t start = center - padding;
    uint16_t end = center + padding;
    if(is_horizontal) {
        LCD_setX(0, (LCD_X_MAX - 1));
        LCD_setY(start, end);
    }
    else {
        LCD_setX(start, end);
        LCD_setY(0, (LCD_Y_MAX - 1));
    }

    LCD_Draw();

    return;
}

void LCD_drawHoriLine(uint16_t yCenter, uint16_t lineWidth) {
    LCD_drawLine(yCenter, lineWidth, true);
    return;
}

void LCD_drawVertLine(uint16_t xCenter, uint16_t lineWidth) {
    LCD_drawLine(xCenter, lineWidth, false);
    return;
}

void LCD_drawRectangle(uint16_t x1, uint16_t dx, uint16_t y1, uint16_t dy, bool isFilled) {
    uint16_t x2;
    uint16_t y2;

    // ensure startRow and startCol are less than their max numbers
    x1 = (x1 < LCD_X_MAX) ? x1 : (LCD_X_MAX - 1);
    y1 = (y1 < LCD_Y_MAX) ? y1 : (LCD_Y_MAX - 1);

    // ensure lines don't go out of bounds
    if((x1 + dx) > LCD_X_MAX) {
        dx = (LCD_X_MAX - x1 - 1);
    }
    if((y1 + dy) > LCD_Y_MAX) {
        dy = (LCD_Y_MAX - y1 - 1);
    }

    // draw rectangle based on `is_filled`
    x2 = (x1 + dx) - 1;
    y2 = (y1 + dy) - 1;
    if(isFilled) {
        LCD_setX(x1, x2);
        LCD_setY(y1, y2);
        LCD_Draw();
    }
    else {
        // left side
        LCD_setX(x1, x2);
        LCD_setY(y1, y1);
        LCD_Draw();

        // right side
        LCD_setY(y2, y2);
        LCD_Draw();

        // top side
        LCD_setX(x1, x1);
        LCD_setY(y1, y2);
        LCD_Draw();

        // right side
        LCD_setX(x2, x2);
        LCD_Draw();
    }

    return;
}

/** @} */               // lcd
