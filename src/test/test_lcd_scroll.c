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

int main(void) {
    uint16_t x_start;
    bool is_increasing;

    PLL_Init();
    Timer0A_Init();
    GPIO_PF_LED_Init();

    LCD_Init();

    ILI9341_setScrollArea(0, X_MAX, 0);

    LCD_setColor_3bit(LCD_RED);     LCD_drawRectangle(0, 0, 45, Y_MAX, 1);
    LCD_setColor_3bit(LCD_YELLOW);  LCD_drawRectangle(45, 0, 45, Y_MAX, 1);
    LCD_setColor_3bit(LCD_GREEN);   LCD_drawRectangle(90, 0, 45, Y_MAX, 1);
    LCD_setColor_3bit(LCD_CYAN);    LCD_drawRectangle(135, 0, 45, Y_MAX, 1);
    LCD_setColor_3bit(LCD_BLUE);    LCD_drawRectangle(180, 0, 45, Y_MAX, 1);
    LCD_setColor_3bit(LCD_PURPLE);  LCD_drawRectangle(225, 0, 45, Y_MAX, 1);
    LCD_setColor_3bit(LCD_WHITE);   LCD_drawRectangle(270, 0, 50, Y_MAX, 1);

    // ILI9341_setScrollStart(240);

    LCD_toggleOutput();                         // display ON
    GPIO_PF_LED_Toggle(0x08);

    x_start = 0;
    is_increasing = true;
    while(1) {
        ILI9341_setScrollStart(x_start);

        x_start = ( is_increasing ) ? (x_start + 1) : (x_start - 1);
        if( x_start == (X_MAX - 1) ) { is_increasing = false; }
        else if(x_start == 0) { is_increasing = true; }

        Timer0A_Wait1ms(10);
    }
}
