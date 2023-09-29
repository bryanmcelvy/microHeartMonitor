#include "GPIO.h"
#include "PLL.h"
#include "SysTick.h"
#include "Timer.h"

#include "ISR.h"

#include <stdbool.h>
#include <stdint.h>

#define LED_PINS (GPIO_Pin_t)(GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3)
#define SW_PINS  (GPIO_Pin_t)(GPIO_PIN0 | GPIO_PIN4)

Timer_t debounceTimer = 0;
volatile uint8_t color;

int main(void) {
    PLL_Init();                         // set bus frequency to 80 MHz
    SysTick_Timer_Init();               // time delays

    debounceTimer = Timer_Init(TIMER0);
    Timer_setMode(debounceTimer, ONESHOT, UP);
    Timer_setInterval_ms(debounceTimer, 100);

    // Init. LED pins
    GPIO_Port_t * portF = GPIO_InitPort(F);
    GPIO_ConfigDirOutput(portF, LED_PINS);
    GPIO_ConfigDriveStrength(portF, LED_PINS, 8);
    GPIO_EnableDigital(portF, LED_PINS);

    // Init. Sw1/2
    GPIO_ConfigDirInput(portF, SW_PINS);
    GPIO_ConfigPullUp(portF, SW_PINS);
    GPIO_ConfigInterrupts_Edge(portF, SW_PINS, false);
    GPIO_ConfigNVIC(portF, 0);
    GPIO_EnableDigital(portF, SW_PINS);

    color = 0x02;
    while(1) {
        GPIO_WriteHigh(portF, color);
        SysTick_Wait1ms(333);
        GPIO_WriteLow(portF, LED_PINS);
        SysTick_Wait1ms(333);
    }
}

/**
 * @brief   ISR for GPIO Port F, trigged when either or both of Sw1 and Sw2 are pressed.
 *
 */
void GPIO_PortF_Handler(void) {
    const uint8_t color_table[6] = { 0x02, 0x06, 0x04, 0x0C, 0x08, 0x0A };
    static uint8_t color_idx = 0;
    uint8_t int_mask;

    int_mask = GPIO_PORTF_RIS_R & 0x11;
    if(Timer_isCounting(debounceTimer) == false) {
        switch(int_mask) {                        // determine interrupt source
            case(0x11):                           // both pressed
            case(0x01):                           // Sw2 pressed (PF0)
                color_idx = (color_idx + 1) % 6;
                break;
            case(0x10):                           // Sw1 pressed (PF4)
                color_idx = (color_idx > 0) ? (color_idx - 1) : 5;
                break;
        }
        color = color_table[color_idx];
        Timer_Start(debounceTimer);               // debounce
    }

    // Acknowledge interrupt by clearing flag
    GPIO_PORTF_ICR_R |= 0x11;               // acknowledge interrupt
}
