/**
 * @file    test_pll.c
 * @author  Bryan McElvy
 * @brief   Test script for the PLL module
 */

#include "PLL.h"
#include "SysTick.h"
#include "tm4c123gh6pm.h"

#define RED   (uint8_t) 0x02
#define BLUE  (uint8_t) 0x04
#define GREEN (uint8_t) 0x08

void GPIO_PortF_Init(void);

int main() {

    uint8_t color;

    PLL_Init();                    // set bus frequency to 80 MHz
    SysTick_Timer_Init();
    GPIO_PortF_Init();

    color = RED + GREEN;
    while(1) {
        // Blink
        GPIO_PORTF_DATA_R |= color;
        SysTick_Wait1ms(500);
        GPIO_PORTF_DATA_R &= ~color;
        SysTick_Wait1ms(500);

        // Decide next color
        switch(color) {
        case(RED + GREEN): color = GREEN + BLUE; break;
        case(GREEN + BLUE): color = RED + BLUE; break;
        case(RED + BLUE): color = RED + GREEN; break;
        }
    }
}

void GPIO_PortF_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x20;

    GPIO_PORTF_LOCK_R = 0x4C4F434B;                    // Unlock GPIO Port F
    GPIO_PORTF_CR_R = 0x0F;                            // Allow changes to PF0
    GPIO_PORTF_AMSEL_R = 0x00;                         // Disable analog on Port F
    GPIO_PORTF_PCTL_R = 0x00000000;                    // Clear port control register
    GPIO_PORTF_DIR_R = 0x0E;                           // Set PF1-3 as output
    GPIO_PORTF_AFSEL_R = 0x00;                         // Disable alt. function
    GPIO_PORTF_DR8R_R |= 0x0E;                         // Set drive strength to 8[mA] for PF1-3
    GPIO_PORTF_DEN_R = 0x0E;                           // Enable digital I/O on PF1-3
    GPIO_PORTF_DATA_R &= ~0x0E;                        // clear PF1-3
}
