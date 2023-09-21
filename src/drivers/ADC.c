/**
 * @addtogroup  adc
 * @{
 *
 * @file
 * @author      Bryan McElvy
 * @brief       Source code for ADC module.
 */

#include "ADC.h"

#include "lookup.h"
#include "GPIO.h"
#include "Timer.h"

#include "arm_math_types.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>

static const float32_t * ADC_LOOKUP = 0;

void ADC_Init(void) {
    // enable clock to ADC0 and wait for it to be ready
    SYSCTL_RCGCADC_R |= 0x01;
    while((SYSCTL_PRADC_R & 0x01) == 0) {}

    // configure GPIO port
    GPIO_Port_t * portE = GPIO_InitPort(E);
    GPIO_ConfigDirInput(portE, GPIO_PIN5);
    GPIO_ConfigAltMode(portE, GPIO_PIN5);
    GPIO_DisableDigital(portE, GPIO_PIN5);
    GPIO_ConfigAnalog(portE, GPIO_PIN5);

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

float32_t ADC_ConvertToVolts(uint16_t raw_sample) {
    return (volatile float32_t) ADC_LOOKUP[raw_sample];
}

/** @} */
