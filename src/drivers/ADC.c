/**
 * @file
 * @author Bryan McElvy
 */

#include "ADC.h"

void ADC_Init(void) {
    SYSCTL_RCGCADC_R |= 0x01;               // enable clock to ADC0
    SYSCTL_RCGCGPIO_R |= 0x10;              // enable clock to GPIO Port E

    GPIO_PORTE_DIR_R &= ~(0x0C);            // PE2/3 as input
    GPIO_PORTE_AFSEL_R |= 0x0C;             // alt. mode for PE2/3
    GPIO_PORTE_DEN_R &= ~(0x0C);            // disable digital I/O for PE2/3
    GPIO_PORTE_AMSEL_R |= 0x0C;             // enable analog mode

    ADC0_PC_R |= 0x01;                      // max f_s = 125 [Hz]
    ADC0_SSPRI_R = ( ADC0_SSPRI_R           // give SS3 highest priority
                        & ~(0x3000) ) 
                        | 0x0123;
    ADC0_ACTSS_R &= ~(0x0F);                // disable all sequencers
    ADC0_EMUX_R |= 0x5000;                  // timer-triggered SS3 interrupt
    ADC0_SSMUX3_R &= ~(0x0F);               // differential pair 0 (Ain0 and Ain1)
    ADC0_SSCTL3_R |= 0x07;                  // disable temp. sensor, enable differential input, enable interrupts
    ADC0_ISC_R |= 0x08;                     // clear SS3 interrupt flag
    ADC0_IM_R |= 0x08;                      // enable SS3 interrupts
    ADC0_ACTSS_R |= 0x08;                   // enable SS3
}

/*
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
*/
