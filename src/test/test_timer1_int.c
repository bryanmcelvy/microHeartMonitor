/**
 * @file    test_timer1_int.c
 * @author  Bryan McElvy
 * @brief   Test script for Timer1A interrupts
 */

#include "GPIO.h"
#include "PLL.h"
#include "Timer.h"
#include "tm4c123gh6pm.h"

uint8_t is_led_on = 0;

int main() {
    InterruptGlobal_Disable();

    PLL_Init();
    Timer1A_Init(200);
    GPIO_PF_LED_Init();
    GPIO_PF_LED_Write(0x0E, 0);                        // turn off all LEDs

    InterruptGlobal_Enable();

    while(1) {}
}

void Timer1A_Handler(void) {
    const uint8_t color_table[6] = { 0x02, 0x06, 0x04, 0x0C, 0x08, 0x0A };
    static uint8_t color_idx = 0;

    if(is_led_on) {
        GPIO_PF_LED_Write(0x0E, 0);                    // turn off all LEDs
    }
    else {
        color_idx = (color_idx < 5) ? (color_idx + 1) : 0;
        GPIO_PF_LED_Toggle(color_table[color_idx]);
    }
    is_led_on = !is_led_on;

    TIMER1_ICR_R |= 0x01;                              // acknowledge interrupt
}
