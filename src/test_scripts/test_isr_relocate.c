/**
 * @file    test_timer1_int.c
 * @author  Bryan McElvy
 * @brief   Test script for relocating the vector table to RAM.
 */

#include "Led.h"

#include "GPIO.h"
#include "PLL.h"
#include "Timer.h"

#include "ISR.h"

#include "tm4c123gh6pm.h"

#include <stdbool.h>
#include <stdint.h>

void Timer_Handler(void);

const GPIO_Pin_t LED_PIN = GPIO_PIN3;

GpioPort_t portF = 0;
Led_t led = 0;
Timer_t timer1 = 0;

int main(void) {
    PLL_Init();

    // Config. interrupt table
    ISR_GlobalDisable();
    ISR_InitNewTableInRam();
    ISR_addToIntTable(Timer_Handler, INT_TIMER1A);
    ISR_setPriority(INT_TIMER1A, 2);
    ISR_Enable(INT_TIMER1A);

    // Init. LED pins
    portF = GPIO_InitPort(F);
    led = Led_Init(portF, LED_PIN);
    Led_TurnOff(led);

    // Init. timer w/ periodic interrupts
    timer1 = Timer_Init(TIMER1);
    Timer_setMode(timer1, PERIODIC, UP);
    Timer_enableInterruptOnTimeout(timer1);

    Timer_setInterval_ms(timer1, 200);
    Timer_Start(timer1);

    ISR_GlobalEnable();
    while(1) {}
}

void Timer_Handler(void) {
    Led_Toggle(led);
    Timer_clearInterruptFlag(timer1);
    return;
}
