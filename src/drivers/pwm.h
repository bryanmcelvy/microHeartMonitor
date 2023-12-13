#ifndef PWM_H
#define PWM_H

#include "GPIO.h"

typedef struct PwmStruct_t * Pwm_t;

typedef enum {
    M0PWM0,               // PB6
    M0PWM1,               // PB7
    M0PWM2,               // PB4
    M0PWM3,               // PB5
    M0PWM4,               // PE4
    M0PWM5,               // PE5
    M0PWM6,               // PC4/PD0
    M0PWM7,               // PC5/PD1
    M1PWM0,               // PD0
    M1PWM1,               // PD1
    M1PWM2,               // PA6/PE4
    M1PWM3,               // PA7/PE5
    M1PWM4,               // PF0
    M1PWM5,               // PF1
    M1PWM6,               // PF2
    M1PWM7,               // PF3
} PwmPin_e;

Pwm_t Pwm_Init(GpioPort_t gpioPort, PwmPin_e pwmPin);

void Pwm_setFrequency(Pwm_t pwm, uint32_t freq);

void Pwm_setDutyCycle(Pwm_t pwm, uint32_t dutyCycle);

uint32_t Pwm_getFrequency(Pwm_t pwm);

uint32_t Pwm_getDutyCycle(Pwm_t pwm);

void Pwm_Enable(Pwm_t pwm);

void Pwm_Disable(Pwm_t pwm);

#endif                    // PWM_H
