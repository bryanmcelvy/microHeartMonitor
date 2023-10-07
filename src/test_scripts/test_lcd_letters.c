#include "LCD.h"

#include "Led.h"

#include "GPIO.h"
#include "PLL.h"
#include "Timer.h"

#define SCALE_FACTOR 10
#define SQUARE_SIZE  6

int main(void) {
    PLL_Init();

    Timer_t timer = Timer_Init(TIMER0);
    Timer_setMode(timer, ONESHOT, UP);

    GPIO_Port_t * portF = GPIO_InitPort(F);
    Led_t led = Led_Init(portF, LED_GREEN);

    LCD_Init();
    LCD_setOutputMode(false);

    LCD_setColor(LCD_BLACK_INV);
    LCD_Fill();

    // Draw H
    LCD_setColor(LCD_RED_INV);
    for(uint8_t i = 0; i < 8; i++) {
        LCD_drawRectangle(0 * SQUARE_SIZE, SQUARE_SIZE, i * SQUARE_SIZE, SQUARE_SIZE, true);
        LCD_drawRectangle(4 * SQUARE_SIZE, SQUARE_SIZE, i * SQUARE_SIZE, SQUARE_SIZE, true);
    }

    // Draw R

    LCD_setOutputMode(true);
    while(1) {
        Timer_Wait1ms(timer, 500);
        Led_Toggle(led);
    }
}
