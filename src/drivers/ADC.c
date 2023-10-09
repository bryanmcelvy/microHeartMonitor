/**
 * @addtogroup  adc
 * @{
 *
 * @file
 * @author      Bryan McElvy
 * @brief       Source code ffor analog-to-digital conversion (ADC) module.
 */

#include "ADC.h"

#include "GPIO.h"

#include "tm4c123gh6pm.h"

void ADC_Init(void) {
    // enable clock to ADC0 and wait for it to be ready
    SYSCTL_RCGCADC_R |= 0x01;
    while((SYSCTL_PRADC_R & 0x01) == 0) {}

    // configure GPIO port
    GpioPort_t portE = GPIO_InitPort(E);
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
    ADC0_SSCTL3_R = 0x06;               // disable temp. sensor, enable interrupts
    ADC0_ISC_R |= 0x08;                 // clear interrupt flag
    ADC0_IM_R |= 0x08;                  // enable interrupt

    ADC0_ACTSS_R |= 0x08;               // enable SS3
    return;
}

/** @} */
