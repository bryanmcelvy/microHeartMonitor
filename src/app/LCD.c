/**
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for LCD module.
 * @ingroup lcd
 */

/******************************************************************************
SECTIONS
        1) Preprocessor Directives & Declarations
        2) Initialization/Configuration
        3) Drawing Area
        4) Color
        5) Drawing
        6) Scrolling
*******************************************************************************/

/******************************************************************************
1) Preprocessor Directives & Declarations
*******************************************************************************/
#include "LCD.h"

#include "ILI9341.h"

#include "SPI.h"
#include "Timer.h"

#include "tm4c123gh6pm.h"

#include <stdint.h>
#include <stdbool.h>

/// @brief Updates `lcd`'s `numPixels` parameter after changing rows/columns
static void LCD_updateNumPixels(void);

/**
 * @brief                   Set new `x` or `y` parameters, and optionally update `numPixels`.
 *
 * @param d1                start index of selected dimension
 * @param d2                end index of selected dimension
 * @param is_x              `true` if dimension is `x`, `false` if `y`
 * @param update_num_pixels `true` to update `lcd.numPixels`, `false` if not
 */
inline static void LCD_setDim(uint16_t d1, uint16_t d2, bool is_x, bool update_num_pixels);

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

typedef struct {
    uint16_t x1;
    uint16_t x2;
    uint16_t y1;
    uint16_t y2;
    uint32_t numPixels;

    uint8_t R_val;
    uint8_t G_val;
    uint8_t B_val;

    bool is_outputON;
    bool is_inverted;
    bool is_16bit;
    bool is_init;
} LCD_t;

static LCD_t lcd = {
    0,
    (X_MAX - 1),                        // entire area
    0,                                  // colStart
    (Y_MAX - 1),                        // colEnd
    (X_MAX * Y_MAX),                    // numPixels

    255,
    255,
    255,                                // default write color is white

    false,                              // display output OFF
    false,                              // color inversion OFF
    true,                               // 16-bit color depth
    false                               // not initialized
};

/******************************************************************************
2) Initialization/Configuration
*******************************************************************************/

void LCD_Init(void) {
    if(lcd.is_init == false) {
        ILI9341_Init();
        ILI9341_setSleepMode(false);
        ILI9341_setMemAccessCtrl(1, 0, 0, 0, 1, 0);
        ILI9341_setColorDepth(true);
        ILI9341_setDispMode(true, false);

        lcd.is_init = true;
    }
    LCD_fill();
}

void LCD_toggleOutput(void) {
    lcd.is_outputON = (lcd.is_outputON) ? false : true;
    ILI9341_setDispOutput(lcd.is_outputON);
}

void LCD_toggleInversion(void) {
    lcd.is_inverted = (lcd.is_inverted) ? false : true;
    ILI9341_setDispInversion(lcd.is_inverted);
}

void LCD_toggleColorDepth(void) {
    lcd.is_16bit = !(lcd.is_16bit);
    ILI9341_setColorDepth(lcd.is_16bit);
    if(lcd.is_16bit) {                    // convert R and B to 5-bits
        lcd.R_val |= 0x1F;
        lcd.B_val |= 0x1F;
    }
}

/******************************************************************************
3) Drawing Area
*******************************************************************************/

static void LCD_updateNumPixels(void) {
    lcd.numPixels = (uint32_t) ((lcd.x2 - lcd.x1) + 1) * ((lcd.y2 - lcd.y1) + 1);
}

inline static void LCD_setDim(uint16_t d1, uint16_t d2, bool is_x, bool update_num_pixels) {
    uint16_t DIM_MAX;

    // ensure the dim numbers meet the restrictions
    DIM_MAX = (is_x) ? X_MAX : Y_MAX;
    d2 = (d2 < DIM_MAX) ? d2 : (DIM_MAX - 1);
    d1 = (d1 <= d2) ? d1 : d2;

    if(is_x) {
        lcd.x1 = d1;
        lcd.x2 = d2;
        ILI9341_setRowAddress(lcd.x1, lcd.x2);
    }
    else {
        lcd.y1 = d1;
        lcd.y2 = d2;
        ILI9341_setColAddress(lcd.y1, lcd.y2);
    }

    if(update_num_pixels) {
        LCD_updateNumPixels();
    }
}

void LCD_setArea(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2) {
    LCD_setDim(x1, x2, true, false);
    LCD_setDim(y1, y2, false, true);
}

void LCD_setX(uint16_t x1, uint16_t x2) {
    LCD_setDim(x1, x2, true, true);
}

void LCD_setY(uint16_t y1, uint16_t y2) {
    LCD_setDim(y1, y2, false, true);
}

/******************************************************************************
4) Color
*******************************************************************************/

void LCD_setColor(uint8_t R_val, uint8_t G_val, uint8_t B_val) {
    lcd.R_val = (lcd.is_16bit) ? (R_val & 0x1F) : (R_val & 0x3F);
    lcd.G_val = G_val & 0x3F;
    lcd.B_val = (lcd.is_16bit) ? (B_val & 0x1F) : (B_val & 0x3F);
}

void LCD_setColor_3bit(uint8_t color_code) {
    // clang-format off
    /**
     *  This is simply a convenience function for setting the color using the
     *  macros defined in the header file.

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

    if(color_code == LCD_BLACK) {
        LCD_setColor(1, 1, 1);
    }
    else {
        LCD_setColor(0x3F * (color_code & 0x04), 0x3F * (color_code & 0x02),
                     0x3F * (color_code & 0x01));
    }
}

/******************************************************************************
5) Drawing
*******************************************************************************/

void LCD_draw(void) {
    /// @showrefs
    ILI9341_writeMemCmd();
    for(int count = 0; count < lcd.numPixels; count++) {
        ILI9341_write1px(lcd.R_val, lcd.G_val, lcd.B_val, lcd.is_16bit);
    }
}

void LCD_fill(void) {
    // LCD_setArea(0, X_MAX, 0, Y_MAX);
    LCD_setDim(0, X_MAX, true, false);
    LCD_setDim(0, Y_MAX, false, true);
    LCD_draw();
}

inline static void LCD_drawLine(uint16_t center, uint16_t lineWidth, bool is_horizontal) {
    uint16_t start, end;
    uint16_t padding;
    uint16_t MAX_NUM;

    MAX_NUM = (is_horizontal) ? Y_MAX : X_MAX;

    // ensure `lineWidth` is odd and positive
    lineWidth = (lineWidth > 0) ? lineWidth : 1;
    lineWidth = ((lineWidth % 2) == 0) ? lineWidth : (lineWidth - 1);
    padding = ((lineWidth - 1) / 2);

    // ensure line does not go out-of-bounds
    if(center < padding) {
        center = padding + 1;
    }
    else if(center >= (MAX_NUM - padding)) {
        center = (MAX_NUM - padding) - 1;
    }

    // set start and end row/column, and draw
    start = center - padding;
    end = center + padding;
    if(is_horizontal) {
        // NOTE: this is split into separate func calls to reduce call stack usage
        LCD_setDim(0, (X_MAX - 1), true, false);
        LCD_setDim(start, end, false, true);
    }
    else {
        // NOTE: see above comment
        LCD_setDim(start, end, true, false);
        LCD_setDim(0, (Y_MAX - 1), false, true);
    }
    LCD_draw();
}

void LCD_drawHLine(uint16_t yCenter, uint16_t lineWidth) {
    LCD_drawLine(yCenter, lineWidth, true);
}

void LCD_drawVLine(uint16_t xCenter, uint16_t lineWidth) {
    LCD_drawLine(xCenter, lineWidth, false);
}

void LCD_drawRectangle(uint16_t x1, uint16_t dx, uint16_t y1, uint16_t dy, bool is_filled) {
    uint16_t x2;
    uint16_t y2;

    // ensure startRow and startCol are less than their max numbers
    x1 = (x1 < X_MAX) ? x1 : (X_MAX - 1);
    y1 = (y1 < Y_MAX) ? y1 : (Y_MAX - 1);

    // ensure lines don't go out of bounds
    if((x1 + dx) > X_MAX) {
        dx = (X_MAX - x1 - 1);
    }
    if((y1 + dy) > Y_MAX) {
        dy = (Y_MAX - y1 - 1);
    }

    // draw rectangle based on `is_filled`
    x2 = (x1 + dx) - 1;
    y2 = (y1 + dy) - 1;
    if(is_filled) {
        // LCD_setArea(x1, x2, y1, y2);
        LCD_setDim(x1, x2, true, false);
        LCD_setDim(y1, y2, false, true);
        LCD_draw();
    }
    else {
        // left side
        LCD_setDim(x1, x2, true, false);
        LCD_setDim(y1, y1, false, true);
        LCD_draw();

        // right side
        LCD_setDim(y2, y2, false, true);
        LCD_draw();

        // top side
        LCD_setDim(x1, x1, true, false);
        LCD_setDim(y1, y2, false, true);
        LCD_draw();

        // right side
        LCD_setDim(x2, x2, true, true);
        LCD_draw();
    }
}

/******************************************************************************
6) Scrolling
*******************************************************************************/

void LCD_drawRectBlank(uint16_t x1, uint16_t dx, uint16_t y1, uint16_t dy, uint16_t y_min,
                       uint16_t y_max, uint16_t color_code) {
    /// TODO: Write description

    uint16_t x2;
    uint16_t y2;

    // set area of display to write to
    x2 = (x1 + dx) - 1;
    y2 = (y1 + dy) - 1;
    // LCD_setArea(x1, x2, y_min, y_max);
    LCD_setDim(x1, x2, true, false);
    LCD_setDim(y_min, y_max, false, true);

    ILI9341_writeMemCmd();

    // write column by column
    for(int x_i = 0; x_i < dx; x_i++) {
        uint16_t numPixels;                    // declared in this loop to get cppcheck to stop
                                               // complaining

        // write blank pixels from `(x1 + x_i, y_min)` to `(x1 + x_i, y1 - 1)
        LCD_setColor_3bit(LCD_WHITE);

        numPixels = y1 - y_min;
        for(int y_i = 0; y_i < numPixels; y_i++) {
            ILI9341_write1px(lcd.R_val, lcd.G_val, lcd.B_val, true);
        }

        // write colored pixels from `(x1 + x_i, y1)` to `(x1 + x_i, y2)`
        LCD_setColor_3bit(color_code);

        numPixels = dy;
        for(int y_i = 0; y_i < numPixels; y_i++) {
            ILI9341_write1px(lcd.R_val, lcd.G_val, lcd.B_val, true);
        }

        // write blank pixels from `(x1 + x_i, y2 + 1)` to `(x1 + x_i, y_max)`
        LCD_setColor_3bit(LCD_WHITE);

        numPixels = y_max - (y2 + 1) + 1;
        for(int y_i = 0; y_i < numPixels; y_i++) {
            ILI9341_write1px(lcd.R_val, lcd.G_val, lcd.B_val, true);
        }
    }
}
