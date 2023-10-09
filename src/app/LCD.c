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
        Plotting Parameters
        Drawing
*******************************************************************************/

#include "ILI9341.h"

#include "Timer.h"

#include "NewAssert.h"

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

    uint16_t lineNum;
    uint16_t colNum;

    uint8_t R_val;               ///< 5 or 6-bit R value
    uint8_t G_val;               ///< 6-bit G value
    uint8_t B_val;               ///< 5 or 6-bit B value

    bool isInit;                 ///< if `true`, LCD has been initialized
} lcd = { 0 };

extern const uint8_t * const FONT_ARRAY[26];

/******************************************************************************
Initialization
*******************************************************************************/

void LCD_Init(void) {
    Assert(lcd.isInit == false);

    Timer_t timer2 = Timer_Init(TIMER2);
    ILI9341_Init(timer2);
    ILI9341_setSleepMode(SLEEP_OFF, timer2);
    Timer_Deinit(timer2);

    ILI9341_setMemAccessCtrl(1, 0, 0, 0, 1, 0);               // TODO: explain this

    ILI9341_setColorDepth(COLORDEPTH_16BIT);
    ILI9341_setColorExpression(PARTIAL_COLORS);
    ILI9341_setDisplayArea(NORMAL_AREA);
    ILI9341_setDispInversion(INVERT_ON);
    ILI9341_setDispOutput(OUTPUT_OFF);

    lcd.isInit = true;

    LCD_setColor(LCD_BLACK);
    LCD_Fill();                                               // black background

    return;
}

void LCD_setOutputMode(bool isOn) {
    outputMode_t outputMode = (isOn) ? OUTPUT_ON : OUTPUT_OFF;
    ILI9341_setDispOutput(outputMode);

    return;
}

/******************************************************************************
Plotting Parameters
*******************************************************************************/

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
    if(color == 0) {
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
        ILI9341_writePixel(lcd.R_val, lcd.G_val, lcd.B_val);
    }

    return;
}

void LCD_Fill(void) {
    LCD_setX(0, LCD_X_MAX);
    LCD_setY(0, LCD_Y_MAX);

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
        LCD_setX(0, (LCD_X_MAX));
        LCD_setY(start, end);
    }
    else {
        LCD_setX(start, end);
        LCD_setY(0, (LCD_Y_MAX));
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
    x1 = (x1 <= LCD_X_MAX) ? x1 : LCD_X_MAX;
    y1 = (y1 <= LCD_Y_MAX) ? y1 : LCD_Y_MAX;

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

/**
 * @brief               Plot a sample at coordinates `(x, y)`.
 *
 * @param[in] x         x-coordinate (i.e. sample number) in range `[0, X_MAX]`
 * @param[in] y         y-coordinate (i.e. amplitude) in range `[0, Y_MAX]`
 * @param[in] color     Color to use
 *
 * @see                 LCD_setX(), LCD_setY(), LCD_setColor(), LCD_Draw()
 */
static void LCD_plotSample(uint16_t x, uint16_t y, LCD_Color_t color);

/******************************************************************************
Writing
*******************************************************************************/

enum {
    HEIGHT_CHAR = 8,
    LEN_CHAR = 5,

    NUM_LINES = 30,
    NUM_COLS = 64
};

void LCD_setCursor(uint16_t lineNum, uint16_t colNum) {
    Assert(lineNum < NUM_LINES);
    Assert(colNum < NUM_COLS);

    lcd.lineNum = lineNum * HEIGHT_CHAR;
    lcd.colNum = colNum * LEN_CHAR;

    return;
}

static void LCD_updateCursor(void) {
    uint16_t newLineNum = lcd.lineNum / HEIGHT_CHAR;
    uint16_t newColNum = lcd.colNum / LEN_CHAR;

    newColNum = (newColNum + 1) % NUM_COLS;
    newLineNum = (newColNum == 0) ? ((newLineNum + 1) % NUM_LINES) : newLineNum;

    lcd.lineNum = newLineNum * HEIGHT_CHAR;
    lcd.colNum = newColNum * LEN_CHAR;

    return;
}

void LCD_writeChar(unsigned char inputChar) {
    // determine letter
    Assert((inputChar < 'A') == false);
    inputChar = (inputChar > 'Z') ? (inputChar - 32) : inputChar;
    Assert((inputChar > 'Z') == false);

    uint16_t lineNum = lcd.lineNum;
    uint16_t colNum = lcd.colNum;

    const uint8_t * letter = FONT_ARRAY[inputChar - 65];

    for(uint8_t lineIdx = 0; lineIdx < HEIGHT_CHAR; lineIdx++) {
        uint8_t line = letter[HEIGHT_CHAR - 1 - lineIdx];
        for(uint8_t colIdx = 0; colIdx < LEN_CHAR; colIdx++) {
            uint8_t shiftVal = LEN_CHAR - 1 - colIdx;
            uint8_t pixel = line & (1 << shiftVal);

            LCD_Color_t color = (pixel) ? LCD_RED : LCD_BLACK;
            LCD_plotSample(colNum + colIdx, lineNum + lineIdx, color);
        }
    }
    LCD_updateCursor();
    return;
}

void LCD_writeStr(void * asciiString) {
    unsigned char * str = (unsigned char *) asciiString;
    uint8_t idx = 0;
    while(str[idx] != '\0') {
        LCD_writeChar(str[idx]);
        LCD_updateCursor();
        idx += 1;
    }

    return;
}

/** @} */               // lcd
