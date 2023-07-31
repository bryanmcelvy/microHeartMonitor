/**
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for LCD module.
 */

/******************************************************************************
Includes
*******************************************************************************/
#include "LCD.h"

#include "ILI9341.h"

#include "SPI.h"
#include "Timer.h"

#include "tm4c123gh6pm.h"

#include <stdint.h>

/******************************************************************************
Declarations
*******************************************************************************/

// Function Prototypes
static void LCD_updateNumPixels(void);
static void LCD_drawLine(uint16_t center, uint16_t lineWidth, bool is_row);

// LCD Object
struct LCD {
    uint16_t rowStart;
    uint16_t rowEnd;
    uint16_t colStart;
    uint16_t colEnd;
    uint32_t numPixels;

    uint8_t R_val;
    uint8_t G_val;
    uint8_t B_val;

    bool is_ON;
    bool is_inverted;

    bool is_16bit;
    bool is_init;
};

static struct LCD lcd = {  
    0, (NUM_ROWS-1),                // entire area
    0, (NUM_COLS-1),                // colStart, colEnd
    (NUM_ROWS * NUM_COLS),          // numPixels

    255, 0, 0,                      // write color is red

    true,                           // display is on
    false,                          // not inverted
    true,                           // 16-bit color depth
    false                           // not initialized
};

/******************************************************************************
Initialization and Configuration
*******************************************************************************/

void LCD_Init(void) {
    if (lcd.is_init == false) {
        ILI9341_Init();
        ILI9341_setMemAccessCtrl(0, 0, 0, 0, 1, 0);
        lcd.is_init = true;
    }
}

void LCD_toggleStatus(void) {
    lcd.is_ON = !(lcd.is_ON);
    ILI9341_setDisplayStatus(lcd.is_ON);
}

void LCD_toggleInversion(void) {
    lcd.is_inverted = !(lcd.is_inverted);
    ILI9341_setDispInversion(lcd.is_inverted);
}

void LCD_toggleColorDepth(void) {
    lcd.is_16bit = !(lcd.is_16bit);
    ILI9341_setColorDepth(lcd.is_16bit);
    if (lcd.is_16bit) {                     // convert R and B to 5-bits
        lcd.R_val |= 0x1F;
        lcd.B_val |= 0x1F;
    }
}

/******************************************************************************
Drawing Area
*******************************************************************************/

static void LCD_updateNumPixels(void) {
    lcd.numPixels = ((lcd.rowEnd - lcd.rowStart) + 1) 
                    * ((lcd.colEnd - lcd.colStart) + 1);
}

void LCD_setArea(   uint16_t rowStartNew, uint16_t rowEndNew,
                    uint16_t colStartNew, uint16_t colEndNew) {
    lcd.rowEnd = (rowEndNew < NUM_ROWS) ? rowEndNew : (NUM_ROWS - 1);
    lcd.rowStart = (rowStartNew < rowEndNew) ? rowStartNew : (rowEndNew);

    lcd.colEnd = (colEndNew < NUM_COLS) ? colEndNew : (NUM_COLS - 1);
    lcd.colStart = (colStartNew < colEndNew) ? colStartNew : (colEndNew);

    LCD_updateNumPixels();
}

void LCD_setRow(uint16_t rowStartNew, uint16_t rowEndNew) {
    lcd.rowEnd = (rowEndNew < NUM_ROWS) ? rowEndNew : (NUM_ROWS - 1);
    lcd.rowStart = (rowStartNew < rowEndNew) ? rowStartNew : (rowEndNew);
    LCD_updateNumPixels();
}

void LCD_setCol(uint16_t colStartNew, uint16_t colEndNew) {
    lcd.colEnd = (colEndNew < NUM_COLS) ? colEndNew : (NUM_COLS - 1);
    lcd.colStart = (colStartNew < colEndNew) ? colStartNew : (colEndNew);
    LCD_updateNumPixels();
}

/******************************************************************************
Color
*******************************************************************************/

void LCD_setColor(uint8_t R_val, uint8_t G_val, uint8_t B_val) {
    lcd.R_val = (lcd.is_16bit) ? (R_val & 0x1F) : (R_val & 0x3F);
    lcd.G_val = G_val & 0x3F;
    lcd.B_val = (lcd.is_16bit) ? (B_val & 0x1F) : (B_val & 0x3F);
}

void LCD_setColor_3bit(uint8_t color_code) {
    /**
        This is simply a convenience function for `LCD_setColor()`.
        The following table shows what the output color will be:

        hex | binary | pixel color
        -------------|------------
        0x04|  100   |    red 
        0x06|  110   |    yellow 
        0x02|  010   |    green 
        0x03|  011   |    cyan 
        0x01|  001   |    blue 
        0x05|  101   |    purple 
        0x07|  111   |    white 
     */
    
    // use white if `color_code` == 0
    color_code = (color_code > 0) ? (color_code & 0x07) : 0x07;

    LCD_setColor(   255 * (color_code & 0x04), 
                    255 * (color_code & 0x02),
                    255 * (color_code & 0x01)   );
}

/******************************************************************************
Drawing
*******************************************************************************/

void LCD_draw(void) {
    ILI9341_setRowAddress(lcd.rowStart, lcd.rowEnd);
    ILI9341_setColAddress(lcd.colStart, lcd.colEnd);

    ILI9341_writeMemCmd();
    for (int count = 0; count < lcd.numPixels; count ++) {
        ILI9341_write1px(lcd.R_val, lcd.G_val, lcd.B_val, lcd.is_16bit);
    }
}

static void LCD_drawLine(uint16_t center, uint16_t lineWidth, bool is_row) {
    uint16_t start, end;
    uint16_t padding;
    uint16_t max_num;

    max_num = (is_row) ? NUM_ROWS : NUM_COLS;

    lineWidth = ((lineWidth % 2) == 0) ? lineWidth : (lineWidth - 1);
    padding = ( (lineWidth - 1) / 2 );

    if (center < padding) {
        center = padding + 1;
    }
    else if ( center >= (max_num - padding) ) {
        center = (max_num - padding) - 1;
    }

    start = center - padding;
    end = center + padding;

    if (is_row) {
        LCD_setArea(start, end, 0, (NUM_COLS-1));
    }
    else {
        LCD_setArea(0, (NUM_ROWS-1), start, end);
    }
    LCD_draw();
}

void LCD_drawHLine(uint16_t rowCenter, uint16_t lineWidth) {
    LCD_drawLine(rowCenter, lineWidth, 1);
}

void LCD_drawVLine(uint16_t colCenter, uint16_t lineWidth) {
    LCD_drawLine(colCenter, lineWidth, 0);
}

//TODO: Write
void LCD_drawSquare(uint16_t startRow, uint16_t startCol, uint16_t N) {

}

