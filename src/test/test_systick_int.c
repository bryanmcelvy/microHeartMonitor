/**
 * @file    test_systick_int.c
 * @author  Bryan McElvy
 * @brief   Test script for SysTick interrupts
 */

#include "PLL.h"
#include "SysTick.h"
#include "tm4c123gh6pm.h"

void GPIO_PortF_Init(void);

const uint8_t color_table[6] = {0x02, 0x06, 0x04, 0x0C, 0x08, 0x0A};
volatile uint8_t color_idx = 0;
volatile uint8_t led_is_on = 0;

int main() {
	InterruptGlobal_Disable();

	PLL_Init();
	SysTick_Interrupt_Init(200);			// f_interrupt = 5[Hz]
	GPIO_PortF_Init();
	
	InterruptGlobal_Enable();
	while(1) {}
}

void GPIO_PortF_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x20;              // start clock for Port F

    // GPIO_PORTF_LOCK_R = 0x4C4F434B;	    // Unlock GPIO Port F
	// GPIO_PORTF_CR_R = 0x0F;				// Allow changes to PF0
	GPIO_PORTF_AMSEL_R = 0x00;			    // Disable analog on Port F
	GPIO_PORTF_PCTL_R = 0x00000000;         // Clear port control register

	GPIO_PORTF_DIR_R |= 0x0E;				// Set PF1-3 as output
	GPIO_PORTF_AFSEL_R &= ~0xFF;			// Disable alt. function

    GPIO_PORTF_DR8R_R |= 0x0E;              // Set drive strength to 8[mA] for PF1-3

	GPIO_PORTF_DEN_R |= 0x0E;				// Enable digital I/O on PF1-3
    GPIO_PORTF_DATA_R &= ~0x0E;             // clear PF1-3
}

void SysTick_Handler(void) {
	led_is_on = !led_is_on;
	if (led_is_on) {
		GPIO_PORTF_DATA_R |= color_table[color_idx];
	}
	else {
		GPIO_PORTF_DATA_R &= ~(color_table[color_idx]);
		color_idx = (color_idx < 5) ? (color_idx + 1) : 0;
	}
}
