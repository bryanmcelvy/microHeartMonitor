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
        Declarations
        Initialization
        Plotting Parameters
        Drawing
*******************************************************************************/

#include "ILI9341.h"

#include "Timer.h"

#include "NewAssert.h"

#include <stdbool.h>
#include <stdint.h>

#define CONVERT_INT_TO_ASCII(X) ((unsigned char) (X + 0x30))

/******************************************************************************
Declarations
*******************************************************************************/

/** @name Helper Functions */               /// @{

/**
 * @brief               Helper function for drawing straight lines.
 *
 * @param center        Row or column that the line is centered on.
 *                      `center` is increased or decreased if the line to be
 *                      written would have gone out of bounds.
 * @param lineWidth     Width of the line. Should be a positive, odd number.
 * @param is_row        `true` for horizontal line, `false` for vertical line
 */
static void LCD_drawLine(uint16_t center, uint16_t lineWidth, bool is_horizontal);

/// @brief               Update the cursor for after writing text on the display.
static void LCD_updateCursor(void);

/** @} */                           // Helper Functions

static struct {
    uint16_t x1;                    ///< starting x-value in range [0, x2]
    uint16_t x2;                    ///< ending x-value in range [0, NUM_ROWS)
    uint16_t y1;                    ///< starting y-value in range [0, y2]
    uint16_t y2;                    ///< ending x-value in range [0, NUM_COLS)

    uint16_t lineNum;               ///< line number for text; in range `[0, NUM_LINES)`
    uint16_t colNum;                ///< column number for text; in range `[0, NUM_COLS)`

    uint8_t color;

    bool isInit;                    ///< if `true`, LCD has been initialized
} lcd = { 0 };

extern const uint8_t * const FONT_ARRAY[128];

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

void LCD_setColor(uint8_t color) {
    Assert(color < 0x08);
    lcd.color = color;
    return;
}

/******************************************************************************
Drawing
*******************************************************************************/

void LCD_Draw(void) {
    // determine RGB values
    uint8_t R, G, B;
    if(lcd.color == 0) {
        R = 1;
        G = 1;
        B = 1;
    }
    else {
        R = 0x1F * ((lcd.color & 0x04) >> 2);
        G = 0x3F * ((lcd.color & 0x02) >> 1);
        B = 0x1F * (lcd.color & 0x01);
    }

    uint32_t numPixels = (uint32_t) ((lcd.x2 - lcd.x1) + 1) * ((lcd.y2 - lcd.y1) + 1);
    ILI9341_writeMemCmd();
    for(uint32_t count = 0; count < numPixels; count++) {
        ILI9341_writePixel(R, G, B);
    }

    return;
}

void LCD_Fill(void) {
    LCD_setX(0, LCD_X_MAX);
    LCD_setY(0, LCD_Y_MAX);

    LCD_Draw();

    return;
}

static void LCD_drawLine(uint16_t center, uint16_t lineWidth, bool is_horizontal) {
    Assert(lineWidth > 0);
    Assert((lineWidth % 2) != 0);

    // ensure line does not go out-of-bounds
    uint16_t padding = ((lineWidth - 1) / 2);
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

void LCD_drawRectangle(uint16_t x1, uint16_t dx, uint16_t y1, uint16_t dy) {
    Assert(x1 <= LCD_X_MAX);
    Assert(x1 + dx <= LCD_X_MAX);
    Assert(y1 <= LCD_Y_MAX);
    Assert((y1 + dy) <= LCD_Y_MAX);

    uint16_t x2 = (x1 + dx) - 1;
    uint16_t y2 = (y1 + dy) - 1;
    LCD_setX(x1, x2);
    LCD_setY(y1, y2);
    LCD_Draw();

    return;
}

void LCD_plotSample(uint16_t x, uint16_t y, uint8_t color) {
    uint8_t currColor = lcd.color;
    LCD_setColor(color);

    LCD_setX(x, x);
    LCD_setY(y, y);
    LCD_Draw();

    LCD_setColor(currColor);
    return;
}

/******************************************************************************
Writing
*******************************************************************************/

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

    newColNum = (newColNum + 2) % NUM_COLS;
    newLineNum = (newColNum == 0) ? ((newLineNum + 2) % NUM_LINES) : newLineNum;

    lcd.lineNum = newLineNum * HEIGHT_CHAR;
    lcd.colNum = newColNum * LEN_CHAR;

    return;
}

void LCD_writeChar(unsigned char inputChar) {
    // determine letter
    const uint8_t * letter = FONT_ARRAY[inputChar];
    Assert(((uint32_t) &letter[0]) != 0);

    uint16_t lineNum = lcd.lineNum;
    uint16_t colNum = lcd.colNum;

    for(uint8_t lineIdx = 0; lineIdx < HEIGHT_CHAR; lineIdx++) {
        uint8_t line = letter[HEIGHT_CHAR - 1 - lineIdx];
        for(uint8_t colIdx = 0; colIdx < LEN_CHAR; colIdx++) {
            uint8_t shiftVal = LEN_CHAR - 1 - colIdx;
            uint8_t pixel = line & (1 << shiftVal);

            uint8_t color = (pixel > 0) ? lcd.color : LCD_BLACK;
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
        idx += 1;
    }

    return;
}

void LCD_writeInt(int32_t num) {
    //...
    if(num < 10) {
        LCD_writeChar(CONVERT_INT_TO_ASCII(num));
    }
    else {
        int32_t nearestPowOf10 = 10;
        while(num > (nearestPowOf10 * 10)) {
            nearestPowOf10 *= 10;
        }

        while(nearestPowOf10 > 0) {
            LCD_writeChar(CONVERT_INT_TO_ASCII(num / nearestPowOf10));
            num %= nearestPowOf10;
            nearestPowOf10 /= 10;
        }
    }
}

void LCD_writeFloat(float num) {
    //...
    int32_t intPart = num / (int32_t) 1;
    if(intPart < 100) {
        LCD_writeChar(' ');
    }
    LCD_writeInt(intPart);

    LCD_writeChar('.');

    int32_t decPart = (int32_t) ((num - intPart) * 10);
    LCD_writeChar(CONVERT_INT_TO_ASCII(decPart));

    return;
}

/** @} */               // lcd
