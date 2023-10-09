/**
 * @file    test_timer1_int.c
 * @author  Bryan McElvy
 * @brief   Test script for Timer1A interrupts
 */

#include "GPIO.h"
#include "PLL.h"
#include "Timer.h"

#include "ISR.h"

#include "tm4c123gh6pm.h"

#include <stdbool.h>
#include <stdint.h>

#define LED_PINS (GPIO_Pin_t)(GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3)

GpioPort_t portF = 0;
Timer_t timer1 = 0;
bool isLedOn = false;

int main(void) {
    PLL_Init();
    ISR_GlobalDisable();

    // Init. LED pins
    portF = GPIO_InitPort(F);
    GPIO_ConfigDirOutput(portF, LED_PINS);
    GPIO_ConfigDriveStrength(portF, LED_PINS, 8);
    GPIO_EnableDigital(portF, LED_PINS);
    GPIO_WriteLow(portF, LED_PINS);               // turn off all LEDs

    // Init. timer w/ periodic interrupts
    ISR_setPriority(INT_TIMER1A, 2);
    ISR_Enable(INT_TIMER1A);

    timer1 = Timer_Init(TIMER1);
    Timer_setMode(timer1, PERIODIC, UP);
    Timer_enableInterruptOnTimeout(timer1);
    Timer_setInterval_ms(timer1, 200);
    Timer_Start(timer1);

    ISR_GlobalEnable();
    while(1) {}
}

void Timer1A_Handler(void) {
    const uint8_t colorArray[6] = { 0x02, 0x06, 0x04, 0x0C, 0x08, 0x0A };
    static uint8_t colorIdx = 0;
    portF = GPIO_InitPort(F);

    if(isLedOn) {
        GPIO_WriteLow(portF, LED_PINS);               // turn off all LEDs
    }
    else {
        colorIdx = (colorIdx + 1) % 6;
        GPIO_Toggle(portF, colorArray[colorIdx]);
    }
    isLedOn = !isLedOn;

    Timer_clearInterruptFlag(timer1);
    return;
}
