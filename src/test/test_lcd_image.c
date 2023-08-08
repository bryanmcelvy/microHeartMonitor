/**
 * @file
 * @author  Bryan McElvy
 * @brief   Test script for writing images onto the display.
 */

#include "LCD.h"
#include "GPIO.h"
#include "PLL.h"
#include "Timer.h"

#include "ILI9341.h"

#include "tm4c123gh6pm.h"

#include <stdint.h>
#include <stdbool.h>

#define X_OFFSET (uint16_t) 0
#define SIZE     (uint16_t) 4

const uint8_t COLOR_ARR[6] = { LCD_RED, LCD_YELLOW, LCD_GREEN, LCD_CYAN, LCD_BLUE, LCD_PURPLE };
uint8_t color_idx;

int main(void) {
    uint16_t x, y;
    bool is_increasing;

    PLL_Init();
    Timer0A_Init();
    GPIO_PF_LED_Init();
    LCD_Init();

    x = 0;
    y = 0;
    is_increasing = true;
    color_idx = 0;
    LCD_setColor_3bit(COLOR_ARR[color_idx]);
    LCD_toggleOutput();

    GPIO_PF_LED_Toggle(LED_GREEN);
    while(1) {

        while(Timer0A_isCounting()) {}
        LCD_drawRectangle(x, SIZE, y, SIZE, true);
        Timer0A_Start(10);

        x = ((x + SIZE) < X_MAX) ? (x + SIZE) : 0;

        if((y + SIZE) >= Y_MAX) {
            is_increasing = false;
        }
        else if(y == 0) {
            is_increasing = true;
        }

        y = (is_increasing) ? (y + SIZE) : (y - SIZE);

        if(x == 0) {
            color_idx = (color_idx < 5) ? (color_idx + 1) : 0;
            LCD_setColor_3bit(COLOR_ARR[color_idx]);
        }
    }
}
