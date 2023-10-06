/**
 * @addtogroup lcd
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for LCD module.
 */

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

#include "LCD.h"

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

/// @brief Updates `lcd`'s `numPixels` parameter after changing rows/columns
inline static void LCD_updateNumPixels(void);

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

static struct {
    uint16_t x1;                      ///< starting x-value in range [0, x2]
    uint16_t x2;                      ///< ending x-value in range [0, NUM_ROWS)
    uint16_t y1;                      ///< starting y-value in range [0, y2]
    uint16_t y2;                      ///< ending x-value in range [0, NUM_COLS)
    uint32_t numPixels;               ///< num. of pixels to write; `= (x2-x1 1) * (y2-y1+1)`

    uint8_t R_val;                    ///< 5 or 6-bit R value
    uint8_t G_val;                    ///< 6-bit G value
    uint8_t B_val;                    ///< 5 or 6-bit B value

    bool isOutputOn;                  ///< if `true`, LCD driver writes from its memory to display
    bool isInverted;                  ///< if `true`, the display's colors are inverted
    bool using16bitColors;               ///< `true` for 16-bit color depth, `false` for 18-bit
    bool isInit;                         ///< if `true`, LCD has been initialized
} lcd;

/******************************************************************************
Initialization
*******************************************************************************/

void LCD_Init(void) {
    Assert(lcd.isInit == false);

    Timer_t timer2 = Timer_Init(TIMER2);

    ILI9341_Init(timer2);
    ILI9341_setSleepMode(false, timer2);
    ILI9341_setMemAccessCtrl(1, 0, 0, 0, 1, 0);
    ILI9341_setColorDepth(true);
    ILI9341_setDispMode(true, false);
    // ILI9341_setFrameRateIdle(2, 27);               // frame rate = 35 Hz

    lcd.x1 = 0;
    lcd.x2 = (X_MAX - 1);
    lcd.y1 = 0;
    lcd.y2 = (Y_MAX - 1), lcd.numPixels = (X_MAX * Y_MAX);

    lcd.R_val = 255;
    lcd.G_val = 255;
    lcd.B_val = 255;

    lcd.isOutputOn = false;
    lcd.isInverted = false;
    lcd.using16bitColors = true;

    lcd.isInit = true;

    return;
}

/******************************************************************************
Configuration
*******************************************************************************/

void LCD_setOutputMode(bool isOn) {
    if(isOn != lcd.isOutputOn) {
        LCD_toggleOutput();
    }
    return;
}

void LCD_toggleOutput(void) {
    lcd.isOutputOn = (lcd.isOutputOn) ? false : true;
    ILI9341_setDispOutput(lcd.isOutputOn);

    return;
}

void LCD_setColorInversionMode(bool isOn) {
    if(isOn != lcd.isInverted) {
        LCD_toggleColorInversion();
    }
    return;
}

void LCD_toggleColorInversion(void) {
    lcd.isInverted = (lcd.isInverted) ? false : true;
    ILI9341_setDispInversion(lcd.isInverted);

    return;
}

void LCD_setColorDepth(bool is_16bit) {
    if(is_16bit != lcd.using16bitColors) {
        LCD_toggleColorDepth();
    }
    return;
}

void LCD_toggleColorDepth(void) {
    lcd.using16bitColors = !(lcd.using16bitColors);
    ILI9341_setColorDepth(lcd.using16bitColors);
    if(lcd.using16bitColors) {               // convert R and B to 5-bits
        lcd.R_val |= 0x1F;
        lcd.B_val |= 0x1F;
    }

    return;
}

/******************************************************************************
Drawing Area
*******************************************************************************/

inline static void LCD_updateNumPixels(void) {
    lcd.numPixels = (uint32_t) ((lcd.x2 - lcd.x1) + 1) * ((lcd.y2 - lcd.y1) + 1);
    return;
}

inline static void LCD_setDim(uint16_t d1, uint16_t d2, bool is_x, bool update_num_pixels) {
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

    return;
}

void LCD_setArea(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2) {
    LCD_setDim(x1, x2, true, false);
    LCD_setDim(y1, y2, false, true);
    return;
}

void LCD_setX(uint16_t x1, uint16_t x2) {
    LCD_setDim(x1, x2, true, true);
    return;
}

void LCD_setY(uint16_t y1, uint16_t y2) {
    LCD_setDim(y1, y2, false, true);
    return;
}

/******************************************************************************
Color
*******************************************************************************/

void LCD_setColor(uint8_t R_val, uint8_t G_val, uint8_t B_val) {
    if(lcd.using16bitColors) {
        lcd.R_val = 0x1F * (R_val & 0x04);
        lcd.B_val = 0x1F * (B_val & 0x01);
    }
    else {
        lcd.R_val = 0x3F * (R_val & 0x04);
        lcd.B_val = 0x3F * (B_val & 0x01);
    }
    lcd.G_val = 0x3F * (G_val & 0x02);

    return;
}

void LCD_setColor_3bit(uint8_t color_code) {
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

    if(color_code == LCD_BLACK) {
        lcd.R_val = 1;
        lcd.G_val = 1;
        lcd.B_val = 1;
    }
    else {
        if(lcd.using16bitColors) {
            lcd.R_val = 0x1F * (color_code & 0x04);
            lcd.B_val = 0x1F * (color_code & 0x01);
        }
        else {
            lcd.R_val = 0x3F * (color_code & 0x04);
            lcd.B_val = 0x3F * (color_code & 0x01);
        }
        lcd.G_val = 0x3F * (color_code & 0x02);
    }

    return;
}

/******************************************************************************
Drawing
*******************************************************************************/

void LCD_Draw(void) {
    /// @showrefs
    ILI9341_writeMemCmd();
    for(uint32_t count = 0; count < lcd.numPixels; count++) {
        ILI9341_writePixel(lcd.R_val, lcd.G_val, lcd.B_val, lcd.using16bitColors);
    }

    return;
}

void LCD_Fill(void) {
    // LCD_setArea(0, X_MAX, 0, Y_MAX);
    LCD_setDim(0, X_MAX, true, false);
    LCD_setDim(0, Y_MAX, false, true);
    LCD_Draw();

    return;
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
    if(isFilled) {
        // LCD_setArea(x1, x2, y1, y2);
        LCD_setDim(x1, x2, true, false);
        LCD_setDim(y1, y2, false, true);
        LCD_Draw();
    }
    else {
        // left side
        LCD_setDim(x1, x2, true, false);
        LCD_setDim(y1, y1, false, true);
        LCD_Draw();

        // right side
        LCD_setDim(y2, y2, false, true);
        LCD_Draw();

        // top side
        LCD_setDim(x1, x1, true, false);
        LCD_setDim(y1, y2, false, true);
        LCD_Draw();

        // right side
        LCD_setDim(x2, x2, true, true);
        LCD_Draw();
    }

    return;
}

/******************************************************************************
Scrolling
*******************************************************************************/

void LCD_graphSample(uint16_t x1, uint16_t dx, uint16_t y1, uint16_t dy, uint16_t yMin,
                     uint16_t yMax, uint16_t colorCode) {
    /// TODO: Write description

    uint16_t x2;
    uint16_t y2;

    // set area of display to write to
    x2 = (x1 + dx) - 1;
    y2 = (y1 + dy) - 1;
    LCD_setDim(x1, x2, true, false);                   // using setDim() instead of
    LCD_setDim(yMin, yMax, false, true);               // setArea() to reduce stack usage

    // write column by column
    ILI9341_writeMemCmd();
    for(int x_i = 0; x_i < dx; x_i++) {
        uint16_t numPixels;

        // write blank pixels from `(x1 + x_i, y_min)` to `(x1 + x_i, y1 - 1)
        LCD_setColor_3bit(LCD_WHITE);

        numPixels = y1 - yMin;
        for(int y_i = 0; y_i < numPixels; y_i++) {
            ILI9341_writePixel(lcd.R_val, lcd.G_val, lcd.B_val, true);
        }

        // write colored pixels from `(x1 + x_i, y1)` to `(x1 + x_i, y2)`
        LCD_setColor_3bit(colorCode);

        numPixels = dy;
        for(int y_i = 0; y_i < numPixels; y_i++) {
            ILI9341_writePixel(lcd.R_val, lcd.G_val, lcd.B_val, true);
        }

        // write blank pixels from `(x1 + x_i, y2 + 1)` to `(x1 + x_i, y_max)`
        LCD_setColor_3bit(LCD_WHITE);

        numPixels = yMax - (y2 + 1) + 1;
        for(int y_i = 0; y_i < numPixels; y_i++) {
            ILI9341_writePixel(lcd.R_val, lcd.G_val, lcd.B_val, true);
        }
    }

    return;
}

/** @} */               // lcd
