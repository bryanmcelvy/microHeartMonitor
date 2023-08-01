/**
 * @file
 * @author  Bryan McElvy
 * @brief   Test script for writing different colors on the LCD>
 */

#include "LCD.h"

#include "PLL.h"
#include "GPIO.h"
#include "Timer.h"
#include <stdint.h>

// const uint8_t color_display_arr[7] = {LCD_RED, LCD_YELLOW, LCD_GREEN, LCD_CYAN, LCD_BLUE, LCD_PURPLE, LCD_WHITE};

int main(void) {
    
    // uint8_t colorOutput[3] = {0, 0, 0};
    // uint8_t color_idx1 = 0;
    // uint8_t color_idx2 = 2;

    PLL_Init();
    Timer0A_Init();
    GPIO_PF_LED_Init();
    LCD_Init();
    LCD_toggleStatus();                         // display OFF

    LCD_setColor_3bit(LCD_WHITE);
    LCD_draw();

    LCD_setColor_3bit(LCD_RED);
    LCD_drawRectangle(0, 0, 40, 40, false);

    LCD_setColor_3bit(LCD_BLUE);
    LCD_drawRectangle(0, 199, 40, 40, false);

    LCD_setColor_3bit(LCD_GREEN);
    LCD_drawRectangle(279, 0, 40, 40, false);

    LCD_setColor_3bit(LCD_PURPLE);
    LCD_drawRectangle(279, 199, 40, 40, false);

    LCD_toggleStatus();                         // display ON
    while(1) {
        GPIO_PF_LED_Toggle(0x08);
        Timer0A_Wait1ms(500);
    }
}
