/**
 * @file
 * @author  Bryan McElvy
 * @brief   Test script for writing different colors on the LCD.
 */

#include "LCD.h"

#include "PLL.h"
#include "GPIO.h"
#include "Timer.h"
#include <stdint.h>

#define LED_PINS           (GPIO_Pin_t)(GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3)

#define TOP_LINE_OFFSET    (uint16_t) 180
#define TOP_LINE_THICKNESS (uint16_t) 5

#define DX                 (uint16_t) 5
#define DY                 (uint16_t) 10
#define COL_Y_MIN          (uint16_t) 0
#define COL_Y_MAX          (uint16_t) 177

int main(void) {
    uint16_t x1, y1;
    bool is_y_increasing;

    PLL_Init();
    Timer0A_Init();

    // Init. LED pins
    GPIO_Port_t * portF = GPIO_InitPort(F);
    GPIO_ConfigDirOutput(portF, LED_PINS);
    GPIO_ConfigDriveStrength(portF, LED_PINS, 8);
    GPIO_EnableDigital(portF, LED_PINS);

    LCD_Init();

    LCD_toggleInversion();
    LCD_setColor_3bit(LCD_BLACK);
    LCD_drawHLine(TOP_LINE_OFFSET, TOP_LINE_THICKNESS);

    x1 = 0;
    y1 = 0;
    is_y_increasing = true;

    LCD_toggleOutput();
    GPIO_Toggle(portF, LED_GREEN);
    while(1) {
        // draw pixel
        while(Timer0A_isCounting()) {}
        LCD_graphSample(x1, DX * 2, y1, DY, COL_Y_MIN, COL_Y_MAX, (LCD_WHITE - LCD_RED));
        Timer0A_Start(33);

        // update x1 and y1
        x1 = (x1 + DX < X_MAX) ? (x1 + DX) : 0;
        y1 = (is_y_increasing) ? (y1 + DY) : (y1 - DY);
        y1 = (y1 + DY < COL_Y_MAX) ? y1 : (COL_Y_MAX - DY + 1);

        // change direction as needed
        if(y1 + DY > COL_Y_MAX) {
            is_y_increasing = false;
        }
        if(y1 < DY) {
            is_y_increasing = true;
        }
    }
}
