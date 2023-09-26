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

int main(void) {

    PLL_Init();

    // Init. LED pins
    GPIO_Port_t * portF = GPIO_InitPort(F);
    GPIO_ConfigDirOutput(portF, (GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3));
    GPIO_ConfigDriveStrength(portF, (GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3), 8);
    GPIO_EnableDigital(portF, (GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3));

    GPIO_WriteLow(portF, 0x0E);                       // turn off all LEDs
    Timer1A_Init(200);

    while(1) {}
}

void Timer1A_Handler(void) {
    const uint8_t color_table[6] = { 0x02, 0x06, 0x04, 0x0C, 0x08, 0x0A };
    static uint8_t color_idx = 0;
    GPIO_Port_t * portF_int = GPIO_InitPort(F);

    if(is_led_on) {
        GPIO_WriteLow(portF_int, 0x0E);               // turn off all LEDs
    }
    else {
        color_idx = (color_idx < 5) ? (color_idx + 1) : 0;
        GPIO_Toggle(portF_int, color_table[color_idx]);
    }
    is_led_on = !is_led_on;

    TIMER1_ICR_R |= 0x01;                             // acknowledge interrupt
}
