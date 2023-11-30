/**
 * @addtogroup  i2c
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Header file for inter-integrated circuit (I2C) module.
 */

#ifndef I2C_H
#define I2C_H

#include "GPIO.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct I2C_Struct_t * I2C_t;

typedef enum {
    I2C0,
    I2C1,
    I2C2,
    I2C3
} I2C_Num;

I2C_t I2C_Init(GpioPort_t gpioPort, I2C_Num i2cNum);
bool I2C_isInit(I2C_t i2c);

uint16_t I2C_Read(I2C_t i2c);

#endif                  // I2C_H

/** @} */               // i2c
