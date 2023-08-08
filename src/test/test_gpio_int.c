#include "GPIO.h"
#include "PLL.h"
#include "SysTick.h"
#include "Timer.h"

volatile uint8_t color;

int main() {
    InterruptGlobal_Disable();

    PLL_Init();                                  // set bus frequency to 80 MHz
    SysTick_Timer_Init();                        // time delays
    GPIO_PF_LED_Init();                          // LED output via PF1-3
    GPIO_PF_Interrupt_Init();                    // edge-triggered interrupts via Sw1/2 (PF4/0)
    Timer0A_Init();                              // debouncing

    color = 0x02;
    InterruptGlobal_Enable();

    while(1) {
        GPIO_PF_LED_Write(color, 1);                    // turn on
        SysTick_Wait1ms(500);
        GPIO_PF_LED_Write(0x0E, 0);                     // turn all off
        SysTick_Wait1ms(500);
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
    if(Timer0A_isCounting() == 0) {
        switch(int_mask) {                     // determine interrupt source
        case(0x11):                            // both pressed
        case(0x01):                            // Sw2 pressed (PF0)
            color_idx = (color_idx < 5) ? (color_idx + 1) : 0;
            break;
        case(0x10):                            // Sw1 pressed (PF4)
            color_idx = (color_idx > 0) ? (color_idx - 1) : 5;
            break;
        }
        color = color_table[color_idx];
        Timer0A_Start(100);                    // debounce
    }

    // Acknowledge interrupt by clearing flag
    GPIO_PORTF_ICR_R |= 0x11;                    // acknowledge interrupt
}
