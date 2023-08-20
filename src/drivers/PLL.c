/**
 * @addtogroup pll
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Implementation details for phase-lock-loop (PLL) functions.
 */

#include "PLL.h"

#include "tm4c123gh6pm.h"

#include <stdint.h>

void PLL_Init(void) {

    // Disable PLL and system clock divider
    SYSCTL_RCC_R &= ~(1 << 22);                // disable system clock divider
    SYSCTL_RCC2_R |= 0x80000000;               // use RCC2 register for more freq. options
    SYSCTL_RCC2_R |= (1 << 11);                // set BYPASS2 to disable PLL

    // Select crystal value and oscillator source
    SYSCTL_RCC_R &= ~(0x1F << 6);               // zero out XTAL field
    SYSCTL_RCC_R |= (0x15 << 6);                // 16[MHz] crystal
    SYSCTL_RCC2_R &= ~(0x70);                   // use main oscillator
    SYSCTL_RCC2_R &= ~(0x2000);                 // power on PLL

    // Set system clock divider
    SYSCTL_RCC2_R |= (1 << 30);                   // enable 7-bit divisor
    SYSCTL_RCC2_R &= ~(0x7F << 22);               // clear divisor bits
    SYSCTL_RCC2_R |= (0x04 << 22);                // = (f_PLL / f_bus) - 1
    SYSCTL_RCC_R |= (1 << 22);                    // enable system clock divider

    // Re-activate PLL
    while((SYSCTL_RIS_R & 0x40) == 0) {}               // wait for PLL to lock
    SYSCTL_RCC2_R &= ~(1 << 11);                       // clear BYPASS2 to enable PLL
}

/** @} */
