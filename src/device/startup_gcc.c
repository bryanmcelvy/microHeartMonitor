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
Core Interrupt Handler Declarations
******************************************************************************/

/**
 * @brief Reset handler.
 *
 */
void ResetISR(void);

/**
 * @brief Non-maskable interrupt (NMI) handler.
 *
 */
static void NmiSR(void);

/**
 * @brief Hard fault handler.
 *
 */
static void FaultISR(void);

/**
 * @brief Default interrupt handler.
 *
 */
static void IntDefaultHandler(void);

/******************************************************************************
Interrupt Vector Table
******************************************************************************/

// External declarations for the interrupt handlers used by the application.
void __attribute__((weak, alias("IntDefaultHandler"))) SVC_Handler(void);
void __attribute__((weak, alias("IntDefaultHandler"))) PendSV_Handler(void);
void __attribute__((weak, alias("IntDefaultHandler"))) SysTick_Handler(void);

void __attribute__((weak, alias("IntDefaultHandler"))) ADC0_SS3_Handler(void);
void __attribute__((weak, alias("IntDefaultHandler"))) GPIO_PortF_Handler(void);
void __attribute__((weak, alias("IntDefaultHandler"))) SSI0_Handler(void);
void __attribute__((weak, alias("IntDefaultHandler"))) Timer0A_Handler(void);
void __attribute__((weak, alias("IntDefaultHandler"))) Timer0B_Handler(void);
void __attribute__((weak, alias("IntDefaultHandler"))) Timer1A_Handler(void);
void __attribute__((weak, alias("IntDefaultHandler"))) Timer1B_Handler(void);
void __attribute__((weak, alias("IntDefaultHandler"))) Timer2A_Handler(void);
void __attribute__((weak, alias("IntDefaultHandler"))) Timer2B_Handler(void);
void __attribute__((weak, alias("IntDefaultHandler"))) Timer3A_Handler(void);
void __attribute__((weak, alias("IntDefaultHandler"))) Timer3B_Handler(void);
void __attribute__((weak, alias("IntDefaultHandler"))) Timer4A_Handler(void);
void __attribute__((weak, alias("IntDefaultHandler"))) Timer4B_Handler(void);
void __attribute__((weak, alias("IntDefaultHandler"))) Timer5A_Handler(void);
void __attribute__((weak, alias("IntDefaultHandler"))) Timer5B_Handler(void);
void __attribute__((weak, alias("IntDefaultHandler"))) UART0_Handler(void);

static uint32_t SystemStack[0x200];

// clang-format off

__attribute__((section(".isr_vector"))) void (*const interruptVectorTable[])(void) = {
    (void (*)(void))((uint32_t) SystemStack + sizeof(SystemStack)), // The initial stack pointer
    ResetISR,                              // The reset handler
    NmiSR,                                 // The NMI handler
    FaultISR,                              // The hard fault handler
    IntDefaultHandler,                     // The MPU fault handler
    IntDefaultHandler,                     // The bus fault handler
    IntDefaultHandler,                     // The usage fault handler
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    SVC_Handler,                           // SVCall handler
    IntDefaultHandler,                     // Debug monitor handler
    0,                                     // Reserved
    PendSV_Handler,                        // The PendSV handler
    SysTick_Handler,                       // The SysTick handler
    IntDefaultHandler,                     // GPIO Port A
    IntDefaultHandler,                     // GPIO Port B
    IntDefaultHandler,                     // GPIO Port C
    IntDefaultHandler,                     // GPIO Port D
    IntDefaultHandler,                     // GPIO Port E
    UART0_Handler,                         // UART0 Rx and Tx
    IntDefaultHandler,                     // UART1 Rx and Tx
    SSI0_Handler,                          // SSI0 Rx and Tx
    IntDefaultHandler,                     // I2C0 Master and Slave
    IntDefaultHandler,                     // PWM Fault
    IntDefaultHandler,                     // PWM Generator 0
    IntDefaultHandler,                     // PWM Generator 1
    IntDefaultHandler,                     // PWM Generator 2
    IntDefaultHandler,                     // Quadrature Encoder 0
    IntDefaultHandler,                     // ADC Sequence 0
    IntDefaultHandler,                     // ADC Sequence 1
    IntDefaultHandler,                     // ADC Sequence 2
    ADC0_SS3_Handler,                      // ADC Sequence 3
    IntDefaultHandler,                     // Watchdog timer
    Timer0A_Handler,                       // Timer 0 subtimer A
    Timer0B_Handler,                       // Timer 0 subtimer B
    Timer1A_Handler,                       // Timer 1 subtimer A
    Timer1B_Handler,                       // Timer 1 subtimer B
    Timer2A_Handler,                       // Timer 2 subtimer A
    Timer2B_Handler,                       // Timer 2 subtimer B
    IntDefaultHandler,                     // Analog Comparator 0
    IntDefaultHandler,                     // Analog Comparator 1
    IntDefaultHandler,                     // Analog Comparator 2
    IntDefaultHandler,                     // System Control (PLL, OSC, BO)
    IntDefaultHandler,                     // FLASH Control
    GPIO_PortF_Handler,                    // GPIO Port F
    IntDefaultHandler,                     // GPIO Port G
    IntDefaultHandler,                     // GPIO Port H
    IntDefaultHandler,                     // UART2 Rx and Tx
    IntDefaultHandler,                     // SSI1 Rx and Tx
    Timer3A_Handler,                       // Timer 3 subtimer A
    Timer3B_Handler,                       // Timer 3 subtimer B
    IntDefaultHandler,                     // I2C1 Master and Slave
    IntDefaultHandler,                     // Quadrature Encoder 1
    IntDefaultHandler,                     // CAN0
    IntDefaultHandler,                     // CAN1
    0,                                     // Reserved
    0,                                     // Reserved
    IntDefaultHandler,                     // Hibernate
    IntDefaultHandler,                     // USB0
    IntDefaultHandler,                     // PWM Generator 3
    IntDefaultHandler,                     // uDMA Software Transfer
    IntDefaultHandler,                     // uDMA Error
    IntDefaultHandler,                     // ADC1 Sequence 0
    IntDefaultHandler,                     // ADC1 Sequence 1
    IntDefaultHandler,                     // ADC1 Sequence 2
    IntDefaultHandler,                     // ADC1 Sequence 3
    0,                                     // Reserved
    0,                                     // Reserved
    IntDefaultHandler,                     // GPIO Port J
    IntDefaultHandler,                     // GPIO Port K
    IntDefaultHandler,                     // GPIO Port L
    IntDefaultHandler,                     // SSI2 Rx and Tx
    IntDefaultHandler,                     // SSI3 Rx and Tx
    IntDefaultHandler,                     // UART3 Rx and Tx
    IntDefaultHandler,                     // UART4 Rx and Tx
    IntDefaultHandler,                     // UART5 Rx and Tx
    IntDefaultHandler,                     // UART6 Rx and Tx
    IntDefaultHandler,                     // UART7 Rx and Tx
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    IntDefaultHandler,                     // I2C2 Master and Slave
    IntDefaultHandler,                     // I2C3 Master and Slave
    Timer4A_Handler,                       // Timer 4 subtimer A
    Timer4B_Handler,                       // Timer 4 subtimer B
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    0,                                     // Reserved
    Timer5A_Handler,                       // Timer 5 subtimer A
    Timer5B_Handler,                       // Timer 5 subtimer B
    IntDefaultHandler,                     // Wide Timer 0 subtimer A
    IntDefaultHandler,                     // Wide Timer 0 subtimer B
    IntDefaultHandler,                     // Wide Timer 1 subtimer A
    IntDefaultHandler,                     // Wide Timer 1 subtimer B
    IntDefaultHandler,                     // Wide Timer 2 subtimer A
    IntDefaultHandler,                     // Wide Timer 2 subtimer B
    IntDefaultHandler,                     // Wide Timer 3 subtimer A
    IntDefaultHandler,                     // Wide Timer 3 subtimer B
    IntDefaultHandler,                     // Wide Timer 4 subtimer A
    IntDefaultHandler,                     // Wide Timer 4 subtimer B
    IntDefaultHandler,                     // Wide Timer 5 subtimer A
    IntDefaultHandler,                     // Wide Timer 5 subtimer B
    IntDefaultHandler,                     // FPU
    0,                                     // Reserved
    0,                                     // Reserved
    IntDefaultHandler,                     // I2C4 Master and Slave
    IntDefaultHandler,                     // I2C5 Master and Slave
    IntDefaultHandler,                     // GPIO Port M
    IntDefaultHandler,                     // GPIO Port N
    IntDefaultHandler,                     // Quadrature Encoder 2
    0,                                     // Reserved
    0,                                     // Reserved
    IntDefaultHandler,                     // GPIO Port P (Summary or P0)
    IntDefaultHandler,                     // GPIO Port P1
    IntDefaultHandler,                     // GPIO Port P2
    IntDefaultHandler,                     // GPIO Port P3
    IntDefaultHandler,                     // GPIO Port P4
    IntDefaultHandler,                     // GPIO Port P5
    IntDefaultHandler,                     // GPIO Port P6
    IntDefaultHandler,                     // GPIO Port P7
    IntDefaultHandler,                     // GPIO Port Q (Summary or Q0)
    IntDefaultHandler,                     // GPIO Port Q1
    IntDefaultHandler,                     // GPIO Port Q2
    IntDefaultHandler,                     // GPIO Port Q3
    IntDefaultHandler,                     // GPIO Port Q4
    IntDefaultHandler,                     // GPIO Port Q5
    IntDefaultHandler,                     // GPIO Port Q6
    IntDefaultHandler,                     // GPIO Port Q7
    IntDefaultHandler,                     // GPIO Port R
    IntDefaultHandler,                     // GPIO Port S
    IntDefaultHandler,                     // PWM 1 Generator 0
    IntDefaultHandler,                     // PWM 1 Generator 1
    IntDefaultHandler,                     // PWM 1 Generator 2
    IntDefaultHandler,                     // PWM 1 Generator 3
    IntDefaultHandler                      // PWM 1 Fault
};
// clang-format on

/******************************************************************************
Core Interrupt Handler Definitions
******************************************************************************/

extern uint32_t _ldata;
extern uint32_t _data;                ///< start of data segment
extern uint32_t _edata;               ///< end of data segment
extern uint32_t _bss;                 ///< start of bss segment
extern uint32_t _ebss;                ///< end of bss segment

extern int main(void);

void ResetISR(void) {
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

static void NmiSR(void) {
    while(1) {}
}

static void FaultISR(void) {
    while(1) {}
}

static void IntDefaultHandler(void) {
    while(1) {}
}

/** @} */               // device
