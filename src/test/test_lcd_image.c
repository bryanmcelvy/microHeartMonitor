/**
 * @file
 * @author  Bryan McElvy
 * @brief   Test script for writing images onto the display.
 */

#include "LCD.h"
#include "GPIO.h"
#include "PLL.h"
#include "Timer.h"

#include "tm4c123gh6pm.h"

#include <stdint.h>
#include <stdbool.h>

int main(void) {
    PLL_Init();
    Timer0A_Init();
    GPIO_PF_LED_Init();
    LCD_Init();

    GPIO_PF_LED_Write(0x04, 1);
    while(1) {
        
    }
}
