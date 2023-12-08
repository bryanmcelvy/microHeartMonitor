//*****************************************************************************
//
// startup_gcc.c - Startup code for use with GNU tools.
//
// Copyright (c) 2013-2020 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
//
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the
//   distribution.
//
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision 2.2.0.295 of the Tiva Firmware Development Package.
//
//*****************************************************************************

/**
 * @file
 * @author  Texas Instruments Incorporated.
 * @author  Bryan McElvy
 * @brief   Startup code for use with GNU tools.
 */

#include "tm4c123gh6pm.h"
#include <stdint.h>

/******************************************************************************
Interrupt Handler Declarations
******************************************************************************/

static void Default_Handler(void);

void Reset_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) NMI_Handler(void);
static void HardFault_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) MemManage_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) BusFault_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) UsageFault_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) SVC_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) DebugMon_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) PendSV_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) SysTick_Handler(void);

void __attribute__((weak, alias("Default_Handler"))) GPIO_PortA_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) GPIO_PortB_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) GPIO_PortC_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) GPIO_PortD_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) GPIO_PortE_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) GPIO_PortF_Handler(void);

void __attribute__((weak, alias("Default_Handler"))) ADC0_SS3_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) SSI0_Handler(void);

void __attribute__((weak, alias("Default_Handler"))) Timer0A_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) Timer0B_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) Timer1A_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) Timer1B_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) Timer2A_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) Timer2B_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) Timer3A_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) Timer3B_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) Timer4A_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) Timer4B_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) Timer5A_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) Timer5B_Handler(void);

void __attribute__((weak, alias("Default_Handler"))) UART0_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) UART1_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) UART3_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) UART4_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) UART5_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) UART6_Handler(void);
void __attribute__((weak, alias("Default_Handler"))) UART7_Handler(void);

/******************************************************************************
Interrupt Vector Table
******************************************************************************/

static uint32_t SystemStack[0x200];

__attribute__((section(".isr_vector"))) void (*const interruptVectorTable[])(void) = {
    (void (*)(void))((uint32_t) SystemStack +
                     sizeof(SystemStack)),               // The initial stack pointer
    Reset_Handler,                                       // The reset handler
    NMI_Handler,                                         // The NMI handler
    HardFault_Handler,                                   // The hard fault handler
    MemManage_Handler,                                   // The MPU fault handler
    BusFault_Handler,                                    // The bus fault handler
    UsageFault_Handler,                                  // The usage fault handler
    0,                                                   // Reserved
    0,                                                   // Reserved
    0,                                                   // Reserved
    0,                                                   // Reserved
    SVC_Handler,                                         // SVCall handler
    DebugMon_Handler,                                    // Debug monitor handler
    0,                                                   // Reserved
    PendSV_Handler,                                      // The PendSV handler
    SysTick_Handler,                                     // The SysTick handler
    GPIO_PortA_Handler,                                  // GPIO Port A
    GPIO_PortB_Handler,                                  // GPIO Port B
    GPIO_PortC_Handler,                                  // GPIO Port C
    GPIO_PortD_Handler,                                  // GPIO Port D
    GPIO_PortE_Handler,                                  // GPIO Port E
    UART0_Handler,                                       // UART0 Rx and Tx
    UART1_Handler,                                       // UART1 Rx and Tx
    SSI0_Handler,                                        // SSI0 Rx and Tx
    Default_Handler,                                     // I2C0 Master and Slave
    Default_Handler,                                     // PWM Fault
    Default_Handler,                                     // PWM Generator 0
    Default_Handler,                                     // PWM Generator 1
    Default_Handler,                                     // PWM Generator 2
    Default_Handler,                                     // Quadrature Encoder 0
    Default_Handler,                                     // ADC Sequence 0
    Default_Handler,                                     // ADC Sequence 1
    Default_Handler,                                     // ADC Sequence 2
    ADC0_SS3_Handler,                                    // ADC Sequence 3
    Default_Handler,                                     // Watchdog timer
    Timer0A_Handler,                                     // Timer 0 subtimer A
    Timer0B_Handler,                                     // Timer 0 subtimer B
    Timer1A_Handler,                                     // Timer 1 subtimer A
    Timer1B_Handler,                                     // Timer 1 subtimer B
    Timer2A_Handler,                                     // Timer 2 subtimer A
    Timer2B_Handler,                                     // Timer 2 subtimer B
    Default_Handler,                                     // Analog Comparator 0
    Default_Handler,                                     // Analog Comparator 1
    Default_Handler,                                     // Analog Comparator 2
    Default_Handler,                                     // System Control (PLL, OSC, BO)
    Default_Handler,                                     // FLASH Control
    GPIO_PortF_Handler,                                  // GPIO Port F
    Default_Handler,                                     // GPIO Port G
    Default_Handler,                                     // GPIO Port H
    Default_Handler,                                     // UART2 Rx and Tx
    Default_Handler,                                     // SSI1 Rx and Tx
    Timer3A_Handler,                                     // Timer 3 subtimer A
    Timer3B_Handler,                                     // Timer 3 subtimer B
    Default_Handler,                                     // I2C1 Master and Slave
    Default_Handler,                                     // Quadrature Encoder 1
    Default_Handler,                                     // CAN0
    Default_Handler,                                     // CAN1
    0,                                                   // Reserved
    0,                                                   // Reserved
    Default_Handler,                                     // Hibernate
    Default_Handler,                                     // USB0
    Default_Handler,                                     // PWM Generator 3
    Default_Handler,                                     // uDMA Software Transfer
    Default_Handler,                                     // uDMA Error
    Default_Handler,                                     // ADC1 Sequence 0
    Default_Handler,                                     // ADC1 Sequence 1
    Default_Handler,                                     // ADC1 Sequence 2
    Default_Handler,                                     // ADC1 Sequence 3
    0,                                                   // Reserved
    0,                                                   // Reserved
    Default_Handler,                                     // GPIO Port J
    Default_Handler,                                     // GPIO Port K
    Default_Handler,                                     // GPIO Port L
    Default_Handler,                                     // SSI2 Rx and Tx
    Default_Handler,                                     // SSI3 Rx and Tx
    UART3_Handler,                                       // UART3 Rx and Tx
    UART4_Handler,                                       // UART4 Rx and Tx
    UART5_Handler,                                       // UART5 Rx and Tx
    UART6_Handler,                                       // UART6 Rx and Tx
    UART7_Handler,                                       // UART7 Rx and Tx
    0,                                                   // Reserved
    0,                                                   // Reserved
    0,                                                   // Reserved
    0,                                                   // Reserved
    Default_Handler,                                     // I2C2 Master and Slave
    Default_Handler,                                     // I2C3 Master and Slave
    Timer4A_Handler,                                     // Timer 4 subtimer A
    Timer4B_Handler,                                     // Timer 4 subtimer B
    0,                                                   // Reserved
    0,                                                   // Reserved
    0,                                                   // Reserved
    0,                                                   // Reserved
    0,                                                   // Reserved
    0,                                                   // Reserved
    0,                                                   // Reserved
    0,                                                   // Reserved
    0,                                                   // Reserved
    0,                                                   // Reserved
    0,                                                   // Reserved
    0,                                                   // Reserved
    0,                                                   // Reserved
    0,                                                   // Reserved
    0,                                                   // Reserved
    0,                                                   // Reserved
    0,                                                   // Reserved
    0,                                                   // Reserved
    0,                                                   // Reserved
    0,                                                   // Reserved
    Timer5A_Handler,                                     // Timer 5 subtimer A
    Timer5B_Handler,                                     // Timer 5 subtimer B
    Default_Handler,                                     // Wide Timer 0 subtimer A
    Default_Handler,                                     // Wide Timer 0 subtimer B
    Default_Handler,                                     // Wide Timer 1 subtimer A
    Default_Handler,                                     // Wide Timer 1 subtimer B
    Default_Handler,                                     // Wide Timer 2 subtimer A
    Default_Handler,                                     // Wide Timer 2 subtimer B
    Default_Handler,                                     // Wide Timer 3 subtimer A
    Default_Handler,                                     // Wide Timer 3 subtimer B
    Default_Handler,                                     // Wide Timer 4 subtimer A
    Default_Handler,                                     // Wide Timer 4 subtimer B
    Default_Handler,                                     // Wide Timer 5 subtimer A
    Default_Handler,                                     // Wide Timer 5 subtimer B
    Default_Handler,                                     // FPU
    0,                                                   // Reserved
    0,                                                   // Reserved
    Default_Handler,                                     // I2C4 Master and Slave
    Default_Handler,                                     // I2C5 Master and Slave
    Default_Handler,                                     // GPIO Port M
    Default_Handler,                                     // GPIO Port N
    Default_Handler,                                     // Quadrature Encoder 2
    0,                                                   // Reserved
    0,                                                   // Reserved
    Default_Handler,                                     // GPIO Port P (Summary or P0)
    Default_Handler,                                     // GPIO Port P1
    Default_Handler,                                     // GPIO Port P2
    Default_Handler,                                     // GPIO Port P3
    Default_Handler,                                     // GPIO Port P4
    Default_Handler,                                     // GPIO Port P5
    Default_Handler,                                     // GPIO Port P6
    Default_Handler,                                     // GPIO Port P7
    Default_Handler,                                     // GPIO Port Q (Summary or Q0)
    Default_Handler,                                     // GPIO Port Q1
    Default_Handler,                                     // GPIO Port Q2
    Default_Handler,                                     // GPIO Port Q3
    Default_Handler,                                     // GPIO Port Q4
    Default_Handler,                                     // GPIO Port Q5
    Default_Handler,                                     // GPIO Port Q6
    Default_Handler,                                     // GPIO Port Q7
    Default_Handler,                                     // GPIO Port R
    Default_Handler,                                     // GPIO Port S
    Default_Handler,                                     // PWM 1 Generator 0
    Default_Handler,                                     // PWM 1 Generator 1
    Default_Handler,                                     // PWM 1 Generator 2
    Default_Handler,                                     // PWM 1 Generator 3
    Default_Handler                                      // PWM 1 Fault
};

/******************************************************************************
Core Interrupt Handler Definitions
******************************************************************************/

extern uint32_t _ldata;
extern uint32_t _data;                ///< start of data segment
extern uint32_t _edata;               ///< end of data segment
extern uint32_t _bss;                 ///< start of bss segment
extern uint32_t _ebss;                ///< end of bss segment

extern int main(void);

void Reset_Handler(void) {
    /**
     * This is the code that gets called when the processor first starts execution
     * following a reset event.  Only the absolutely necessary set is performed,
     * after which the application supplied entry() routine is called.  Any fancy
     * actions (such as making decisions based on the reset cause register, and
     * resetting the bits in that register) are left solely in the hands of the
     * application.
     */

    // Copy the data segment initializers from flash to SRAM.
    uint32_t * srcPtr = &_ldata;
    uint32_t * destPtr = &_data;
    while(destPtr < &_edata) {
        *destPtr++ = *srcPtr++;
    }

    // Zero fill the bss segment.
    __asm("    ldr     r0, =_bss\n"
          "    ldr     r1, =_ebss\n"
          "    mov     r2, #0\n"
          "    .thumb_func\n"
          "zero_loop:\n"
          "        cmp     r0, r1\n"
          "        it      lt\n"
          "        strlt   r2, [r0], #4\n"
          "        blt     zero_loop");

    /*
     * Enable the floating-point unit. This must be done here to handle the
     * case where main() uses floating-point and the function prologue saves
     * floating-point registers (which will fault if floating-point is not
     * enabled).
     */
    NVIC_CPAC_R = ((NVIC_CPAC_R & ~(NVIC_CPAC_CP10_M | NVIC_CPAC_CP11_M)) | NVIC_CPAC_CP10_FULL |
                   NVIC_CPAC_CP11_FULL);

    main();
}

static void HardFault_Handler(void) {
    __asm__("BKPT #0");
    while(1) {}
}

static void Default_Handler(void) {
    __asm__("BKPT #0");
    while(1) {}
}

/** @} */               // device
