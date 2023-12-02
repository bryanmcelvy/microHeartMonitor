#ifndef PWM_H
#define PWM_H

#include "GPIO.h"

typedef struct PwmStruct_t * Pwm_t;

typedef enum {
    M0PWM0,
    M0PWM1,
    M0PWM2,
    M0PWM3,
    M0PWM4,
    M0PWM5,
    M0PWM6,
    M0PWM7,
    M1PWM0,
    M1PWM1,
    M1PWM2,
    M1PWM3,
    M1PWM4,
    M1PWM5,
    M1PWM6,
    M1PWM7
} PwmPin_e;

Pwm_t Pwm_Init(GpioPort_t gpioPort, PwmPin_e pwmPin);

void Pwm_setFrequency(Pwm_t pwm, uint32_t freq);

void Pwm_setDutyCycle(Pwm_t pwm, uint32_t dutyCycle);

uint32_t Pwm_getFrequency(Pwm_t pwm);

uint32_t Pwm_getDutyCycle(Pwm_t pwm);

void Pwm_Enable(Pwm_t pwm);

void Pwm_Disable(Pwm_t pwm);

#endif               // PWM_H
