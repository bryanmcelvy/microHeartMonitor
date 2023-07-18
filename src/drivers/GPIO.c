#include "GPIO.h"

void GPIO_PF_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x20;              // start clock for Port F

	GPIO_PORTF_AMSEL_R &= ~(0x1F);			// Disable analog on Port F
	GPIO_PORTF_AFSEL_R &= ~(0x1F);			// Disable alt. function
	GPIO_PORTF_PCTL_R = 0x00000000;         // Clear port control register

}

void GPIO_PF_LED_Init(void) {
	if ((SYSCTL_RCGCGPIO_R & 0x20) == 0) {
		GPIO_PF_Init();
	}

	GPIO_PORTF_DIR_R |= 0x0E;				// PF1-3 as output
	GPIO_PORTF_DR8R_R |= 0x0E;              // Set drive strength to 8[mA] for PF1-3
	GPIO_PORTF_DEN_R |= 0x0E;				// Enable digital I/O on PF1-3
}

void GPIO_PF_LED_Write(uint8_t color_mask, uint8_t turn_on) {
	color_mask &= 0x0E;						// only read PF1-3
	if (turn_on) {
		GPIO_PORTF_DATA_R |= color_mask;
	}
	else {
		GPIO_PORTF_DATA_R &= ~(color_mask);
	}
}

void GPIO_PF_LED_Toggle(uint8_t color_mask) {
	GPIO_PORTF_DATA_R ^= (color_mask & 0x0E);
}

void GPIO_PF_Sw_Init(void) {
	if ((SYSCTL_RCGCGPIO_R & 0x20) == 0) {
		GPIO_PF_Init();
	}
	
	GPIO_PORTF_LOCK_R = 0x4C4F434B;	        // Unlock GPIO Port F
	GPIO_PORTF_CR_R |= 0x01;			    // Allow changes to PF0

	GPIO_PORTF_DIR_R &= ~(0x11);			// PF0/4 as input
	GPIO_PORTF_PUR_R |= 0x11;               // activate pull-up resistors for PF0/4
	GPIO_PORTF_DEN_R |= 0x11;				// Enable digital I/O on PF0-4
}

void GPIO_PF_Interrupt_Init(void) {
	if ((SYSCTL_RCGCGPIO_R & 0x20) == 0) {
		GPIO_PF_Init();
	}
	if ((GPIO_PORTF_DEN_R & 0x11) == 0) {
		GPIO_PF_Sw_Init();
	}

	GPIO_PORTF_IM_R &= ~(0xFF);				// disarm Port F interrupts

	GPIO_PORTF_IS_R |= 0x11;				// use edge-triggered interrupts for PF0/4 (Sw2 and Sw1)
	GPIO_PORTF_IBE_R &= ~(0x11);			// only use 1 edge
	GPIO_PORTF_IEV_R &= ~(0x11);			// falling edge trigger
	GPIO_PORTF_ICR_R |= 0xFF;				// clear interrupt status for PF

	GPIO_PORTF_IM_R |= 0x11;				// arm interrupts for PF0/4
	NVIC_PRI7_R &= ~(0xE00000);				// priority 0
	NVIC_EN0_R |= (1 << 30);				// enable Port F interrupts in NVIC
}
