#include "PLL.h"
#include "ILI9341.h"
#include "Timer.h"
#include "GPIO.h"

#define LED_PINS (GPIO_Pin_t)(GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3)

enum colors { RED = (uint8_t) 0x02, GREEN = (uint8_t) 0x08, BLUE = (uint8_t) 0x04 };

int main(void) {
    unsigned char color;
    uint8_t red_val, green_val, blue_val;

    PLL_Init();
    Timer_t timer0 = Timer_Init(TIMER0);

    // Initialize driver
    ILI9341_Init(timer0);
    ILI9341_setRowAddress(0, ILI9341_NUM_ROWS - 1);
    ILI9341_setColAddress(0, ILI9341_NUM_COLS - 1);
    ILI9341_setMemAccessCtrl(0, 0, 0, 0, 1, 0);

    // Init. LED pins
    GPIO_Port_t * portF = GPIO_InitPort(F);
    GPIO_ConfigDirOutput(portF, LED_PINS);
    GPIO_ConfigDriveStrength(portF, LED_PINS, 8);
    GPIO_EnableDigital(portF, LED_PINS);

    Timer_setMode(timer0, ONESHOT, DOWN);
    Timer_setInterval_ms(timer0, 1000);

    color = BLUE;
    while(1) {
        // Determine next color
        switch(color) {
            case RED:
                red_val = 0;
                green_val = 255;
                blue_val = 0;
                color = GREEN;
                break;
            case GREEN:
                red_val = 0;
                green_val = 0;
                blue_val = 255;
                color = BLUE;
                break;
            case BLUE:
                red_val = 255;
                green_val = 0;
                blue_val = 0;
                color = RED;
                break;
        }
        while(Timer_isCounting(timer0)) {}

        // Write to display
        Timer_Start(timer0);

        GPIO_WriteLow(portF, LED_PINS);
        GPIO_WriteHigh(portF, color);
        ILI9341_writeMemCmd();
        for(uint32_t i = 0; i < 76800; i++) {
            ILI9341_writePixel(red_val, green_val, blue_val, 1);
        }
    }
}
