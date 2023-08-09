/**
 * @addtogroup systick
 * @{
 *
 * @file    SysTick.c
 * @author  Bryan McElvy
 * @brief   Implementation details for SysTick functions.
 */

#include "SysTick.h"

#include "tm4c123gh6pm.h"

#include <stdint.h>

void SysTick_Timer_Init(void) {
    NVIC_ST_CTRL_R &= ~0x01;                          // disable SysTick
    NVIC_ST_RELOAD_R = 79999;                         // t = 1[ms]
    NVIC_ST_CURRENT_R = 0;                            // write to current to clear it
    NVIC_ST_CTRL_R |= 0x05;                           // set core clock, enable SysTick
}

void SysTick_Wait1ms(uint32_t time_ms) {
    NVIC_ST_CURRENT_R = 0;                            // write to current to clear it
    for(uint32_t i = 0; i < time_ms; i++) {
        while((NVIC_ST_CTRL_R & 0x10000) == 0) {}
    }
}

void SysTick_Interrupt_Init(uint32_t time_ms) {
    uint32_t reload_val;

    time_ms = (time_ms > 200) ? 200 : ((uint32_t) time_ms);
    reload_val = (80000 * time_ms) - 1;

    NVIC_ST_CTRL_R &= ~0x01;                          // disable SysTick
    NVIC_ST_RELOAD_R = reload_val;                    // 24-bit reload value
    NVIC_ST_CURRENT_R = 0;                            // write to current to clear it
    NVIC_SYS_PRI3_R |= (2 << 29);                     // priority 2
    NVIC_ST_CTRL_R |= 0x07;                    // enable SysTick with core clock and interrupts
}

/** @} */
