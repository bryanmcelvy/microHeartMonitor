/**
 * @addtogroup  adc
 * @{
 *
 * @file
 * @author      Bryan McElvy
 * @brief       Source code for ADC module.
 */

#include "lookup.h"
#include "Timer.h"

#include "arm_math_types.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>

#define GPIO_PIN_5 ((uint8_t) (1 << 5))

static const float32_t * ADC_LOOKUP = 0;

void ADC_Init(void) {
    SYSCTL_RCGCADC_R |= 0x01;                               // enable clock to ADC0
    SYSCTL_RCGCGPIO_R |= 0x10;                              // enable clock to GPIO Port E

    GPIO_PORTE_DIR_R &= ~(GPIO_PIN_5);                      // PE5 as input
    GPIO_PORTE_AFSEL_R |= GPIO_PIN_5;                       // alt. mode for PE5
    GPIO_PORTE_DEN_R &= ~(GPIO_PIN_5);                      // disable digital I/O for PE5
    GPIO_PORTE_AMSEL_R |= GPIO_PIN_5;                       // enable analog mode

    ADC0_ACTSS_R &= ~(0x0F);                                // disable all sequencers
    ADC0_PC_R = (ADC0_PC_R & ~(0x0F)) | 0x01;               // max f_s = 125 [Hz]
    ADC0_SSPRI_R = (ADC0_SSPRI_R                            // give SS3 highest priority
                    & ~(0x3000)) |
                   0x0123;
    ADC0_EMUX_R |= 0x5000;                                  // set trigger source to Timer3A
    ADC0_SSMUX3_R = 8;                                      // analog input 8 (Ain8 = PE5)
    ADC0_SSCTL3_R = 0x06;                   // disable temp. sensor, enable differential input,
                                            // enable interrupts
    ADC0_ISC_R |= 0x08;                     // clear SS3 interrupt flag
    ADC0_IM_R |= 0x08;                      // enable SS3 interrupt

    NVIC_PRI4_R |= (1 << 13);               // priority 1 for interrupt 17 (ADC0 SS3)
    NVIC_EN0_R |= (1 << 17);                // enable ADC0 SS3 interrupt in NVIC

    ADC0_ACTSS_R |= 0x08;                   // enable SS3

    ADC_LOOKUP = Lookup_GetPtr_ADC();
}

void ADC_InterruptEnable(void) {
    ADC0_IM_R |= 0x08;
}

void ADC_InterruptDisable(void) {
    ADC0_IM_R &= ~(0x08);
}

volatile float32_t ADC_ConvertToVolts(uint16_t raw_sample) {
    return (volatile float32_t) ADC_LOOKUP[raw_sample];
}

/** @} */
