/**
 * @file
 * @author  Bryan McElvy
 * @brief   Implementation details for phase-lock-loop (PLL) functions.
 */

#include "PLL.h"

void PLL_Init(void) {

    // Disable PLL and system clock divider
    SYSCTL_RCC_R &= ~(1 << 22);                             // disable system clock divider
    SYSCTL_RCC2_R |= 0x80000000;                            // use RCC2 register for more freq. options
    SYSCTL_RCC2_R |= (1 << 11);                             // set BYPASS2 to disable PLL
    
    // Select crystal value and oscillator source
    SYSCTL_RCC_R &= ~(0x1F << 6);                           // zero out XTAL field
    SYSCTL_RCC_R |= (0x15 << 6);                            // 16[MHz] crystal
    SYSCTL_RCC2_R &= ~(0x70);                               // use main oscillator
    SYSCTL_RCC2_R &= ~(0x2000);                             // power on PLL

    // Set system clock divider
    SYSCTL_RCC2_R |= (1 << 30);                             // enable 7-bit divisor
    SYSCTL_RCC2_R &= ~(0x7F << 22);                         // clear divisor bits
    SYSCTL_RCC2_R |= (0x04 << 22);                          // = (f_PLL / f_bus) - 1
    SYSCTL_RCC_R |= (1 << 22);                              // enable system clock divider

    // Re-activate PLL
    while ( (SYSCTL_RIS_R & 0x40) == 0 ) {}                 // wait for PLL to lock
    SYSCTL_RCC2_R &= ~(1 << 11);                            // clear BYPASS2 to enable PLL

}

/*
The PLL is configured using direct register writes to the RCC/RCC2 register.
If the RCC2 register is being used, the USERCC2 bit must be set and the appropriate RCC2 bit/field is used.
The steps required to successfully change the PLL-based sys. clk are:
1. Bypass PLL and sys. clk divider by setting the BYPASS bit and clearing the USESYS bit in RCC, thus configuring the MCU to 
    run off a "raw" clk source and allowing for the new PLL config to be validated before switching the sys. clk to the PLL.
2. Select the XTAL value and oscillator source (OSCSRC), and clear the PWRDN bit in RCC/RCC2. 
    Setting the XTAL field automatically pulls valid PLL config data for the appropriate XTAL, 
    and clearing the PWRDN bit powers and enables the PLL and its output.
3. Select the desired sys. divider (SYSDIV) in RCC/RCC2 and set the USESYS bit in RCC. 
    The SYSDIV field determines the sys. frequency for the microcontroller.
4. Wait for the PLL to lock by polling the PLLLRIS bit in the Raw Interrupt Status (RIS) register.
5. Enable use of the PLL by clearing the BYPASS bit in RCC/RCC2.
*/
