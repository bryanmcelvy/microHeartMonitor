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

// const uint8_t color_display_arr[7] = {LCD_RED, LCD_YELLOW, LCD_GREEN, LCD_CYAN, LCD_BLUE, LCD_PURPLE, LCD_WHITE};

int main(void) {
    
    PLL_Init();
    Timer0A_Init();
    GPIO_PF_LED_Init();

    /*
    TFA = VSA = BFA = 80
    setArea(x1 = x2 = 0, y1 = 80, y2 = 160)

    */

    LCD_Init();

    LCD_setColor_3bit(LCD_RED);
    LCD_setArea(0, NUM_ROWS, 0, 79); LCD_draw();
    LCD_setArea(0, NUM_ROWS-1, 160, NUM_COLS-1); LCD_draw();

    LCD_toggleStatus();                         // display ON
    while(1) {
        GPIO_PF_LED_Toggle(0x08);
        Timer0A_Wait1ms(500);
    }
}
