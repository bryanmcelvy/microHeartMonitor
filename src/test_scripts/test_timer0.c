/**
 * @file    test_timer.c
 * @author  Bryan McElvy
 * @brief   Test file for Timer 0.
 */

#include "GPIO.h"
#include "PLL.h"
#include "Timer.h"

#define LED_PINS (GPIO_Pin_t)(GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3)

int main(void) {
    const uint8_t color_table[6] = { 0x02, 0x06, 0x04, 0x0C, 0x08, 0x0A };
    uint8_t color_idx = 0;

    PLL_Init();               // set bus frequency to 80 MHz
    Timer0A_Init();

    // Init. LED pins
    GPIO_Port_t * portF = GPIO_InitPort(F);
    GPIO_ConfigDirOutput(portF, LED_PINS);
    GPIO_ConfigDriveStrength(portF, LED_PINS, 8);
    GPIO_EnableDigital(portF, LED_PINS);

    while(1) {
        for(uint8_t i = 0; i < 2; i++) {
            GPIO_Toggle(portF, color_table[color_idx]);
            Timer0A_Wait1ms(500);
        }
        color_idx = (color_idx < 5) ? (color_idx + 1) : 0;
    }
}
