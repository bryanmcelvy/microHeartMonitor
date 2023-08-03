/**
 * @file
 * @author  Bryan McElvy
 * @brief   Test script for writing different colors on the LCD>
 */

#include "ILI9341.h"
#include "LCD.h"

#include "PLL.h"
#include "GPIO.h"
#include "Timer.h"
#include <stdint.h>

#define OFFSET (uint16_t) 24

enum colors {   red=LCD_RED, yellow=LCD_YELLOW, green=LCD_GREEN, cyan=LCD_CYAN, 
                blue=LCD_BLUE, purple=LCD_PURPLE, white=LCD_WHITE};

int main(void) {
    // uint16_t color;

    PLL_Init();
    Timer0A_Init();
    GPIO_PF_LED_Init();

    LCD_Init();

    ILI9341_setScrollArea(OFFSET, X_MAX-OFFSET, 0);

    LCD_setColor_3bit(red); LCD_drawRectangle(0, 0, 40, Y_MAX, 1);
    LCD_setColor_3bit(yellow); LCD_drawRectangle(40, 0, 40, Y_MAX, 1);
    LCD_setColor_3bit(green); LCD_drawRectangle(80, 0, 40, Y_MAX, 1);
    LCD_setColor_3bit(cyan); LCD_drawRectangle(120, 0, 40, Y_MAX, 1);
    LCD_setColor_3bit(blue); LCD_drawRectangle(160, 0, 40, Y_MAX, 1);
    LCD_setColor_3bit(purple); LCD_drawRectangle(200, 0, 40, Y_MAX, 1);
    LCD_setColor_3bit(white); LCD_drawRectangle(240, 0, 40, Y_MAX, 1);

    // ILI9341_setScrollStart(240);

    LCD_toggleStatus();                         // display ON
    GPIO_PF_LED_Toggle(0x08);
    int i = 0;
    while(1) {
        ILI9341_setScrollStart(i);
        i = ( i < (X_MAX - 1) ) ? (i + 1) : 0;
        Timer0A_Wait1ms(10);
    }
}
