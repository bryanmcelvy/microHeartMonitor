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

#define LED_PINS (GpioPin_t)(GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3)

const uint8_t COLOR_ARR[6] = { LCD_RED, LCD_YELLOW, LCD_GREEN, LCD_CYAN, LCD_BLUE, LCD_PURPLE };
uint8_t color_idx;

int main(void) {
    uint16_t x, y;
    bool is_increasing;

    PLL_Init();
    Timer_t timer = Timer_Init(TIMER0);
    Timer_setMode(timer, ONESHOT, UP);
    Timer_setInterval_ms(timer, 10);

    // Init. LED pins
    GpioPort_t portF = GPIO_InitPort(F);
    GPIO_ConfigDirOutput(portF, LED_PINS);
    GPIO_ConfigDriveStrength(portF, LED_PINS, 8);
    GPIO_EnableDigital(portF, LED_PINS);

    LCD_Init();

    x = 0;
    y = 0;
    is_increasing = true;
    color_idx = 0;
    LCD_setColor(COLOR_ARR[color_idx]);
    LCD_setOutputMode(true);

    GPIO_Toggle(portF, LED_GREEN);
    while(1) {
        while(Timer_isCounting(timer)) {}
        LCD_drawRectangle(x, SIZE, y, SIZE);
        Timer_Start(timer);

        x = ((x + SIZE) < LCD_X_MAX) ? (x + SIZE) : 0;

        if((y + SIZE) >= LCD_Y_MAX) {
            is_increasing = false;
        }
        else if(y == 0) {
            is_increasing = true;
        }

        y = (is_increasing) ? (y + SIZE) : (y - SIZE);

        if(x == 0) {
            color_idx = (color_idx < 5) ? (color_idx + 1) : 0;
            LCD_setColor(COLOR_ARR[color_idx]);
        }
    }
}
