/**
 * @file
 * @author  Bryan McElvy
 * @brief   Implementation for timer module.
 * 
 * @ingroup timer
 */

#include "Timer.h"

#include "tm4c123gh6pm.h"

#include <stdint.h>

/**********************************************************************
Timer0A
***********************************************************************/
void Timer0A_Init(void) {
    SYSCTL_RCGCTIMER_R |= 0x01;             // enable clock for timer 0
    TIMER0_CTL_R &= ~(0x101);               // disable both subtimers
    TIMER0_CFG_R &= ~(0x07);                // 32-bit timer configuration
    TIMER0_TAMR_R |= 0x01;                  // one-shot mode, countdown
}

void Timer0A_Start(uint32_t time_ms) {
    uint32_t reload_val;
    
    TIMER0_CTL_R &= ~(0x101);               // disable Timer 0

    time_ms = (time_ms <= 53000) ? ((uint32_t) time_ms) : 53000;
    reload_val = (80000 * time_ms) - 1;
    TIMER0_TAILR_R = reload_val;

    TIMER0_CTL_R |= 0x101;                  // re-enable Timer 0
}

uint8_t Timer0A_isCounting(void) {
    return TIMER0_CTL_R & 0x101;             // check enable bit (1 if counting, 0 if not)
}

void Timer0A_Wait1ms(uint32_t time_ms) {
    Timer0A_Start(time_ms);
    while (Timer0A_isCounting()) {}
}

/**********************************************************************
Timer0A
***********************************************************************/

void Timer1A_Init(uint32_t time_ms) {
    uint32_t reload_val;
    
    SYSCTL_RCGCTIMER_R |= 0x02;             // enable clock for timer 1
    TIMER1_CTL_R &= ~(0x101);               // disable both subtimers
    TIMER1_CFG_R &= ~(0x07);                // 32-bit timer configuration
    TIMER1_TAMR_R |= 0x02;                  // periodic mode, countdown

    time_ms = (time_ms <= 53000) ? ((uint32_t) time_ms) : 53000;
    reload_val = (80000 * time_ms) - 1;
    TIMER1_TAILR_R = reload_val;

    TIMER1_IMR_R |= 0x01;                   // interrupt on timeout
    NVIC_PRI5_R |= (2 << 13);               // priority 2
    NVIC_EN0_R |= 0x200000;                 // enable Timer 1A interrupts in NVIC

    TIMER1_CTL_R |= 0x101;                  // enable both subtimers
}

/**********************************************************************
Timer2A
***********************************************************************/
void Timer2A_Init(void) {
    SYSCTL_RCGCTIMER_R |= 0x04;             // enable clock for timer 2
    TIMER2_CTL_R &= ~(0x101);               // disable both subtimers
    TIMER2_CFG_R &= ~(0x07);                // 32-bit timer configuration
    TIMER2_TAMR_R |= 0x01;                  // one-shot mode, countdown
}

void Timer2A_Start(uint32_t time_ms) {
    uint32_t reload_val;
    
    TIMER2_CTL_R &= ~(0x101);               // disable Timer 2

    time_ms = (time_ms <= 53000) ? ((uint32_t) time_ms) : 53000;
    reload_val = (80000 * time_ms) - 1;
    TIMER2_TAILR_R = reload_val;

    TIMER2_CTL_R |= 0x101;                  // re-enable Timer 2
}

uint8_t Timer2A_isCounting(void) {
    return TIMER2_CTL_R & 0x101;             // check enable bit (1 if counting, 0 if not)
}

void Timer2A_Wait1ms(uint32_t time_ms) {
    Timer2A_Start(time_ms);
    while (Timer2A_isCounting()) {}
}
