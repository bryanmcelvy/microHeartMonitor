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

    GpioPort_t portF = GPIO_InitPort(F);
    Led_t led = Led_Init(portF, LED_GREEN);

    LCD_Init();
    LCD_setOutputMode(false);

    LCD_setCursor(0, 0);
    LCD_writeStr("Heart Rate: 75.0 bpm");

    // Draw R

    LCD_setOutputMode(true);
    while(1) {
        Timer_Wait1ms(timer, 500);
        Led_Toggle(led);
    }
}
