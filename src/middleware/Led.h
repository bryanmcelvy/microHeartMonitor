/**
 * @file
 * @author  Bryan McElvy
 * @brief   Interface for LED module.
 * @ingroup led
 */

/** @addtogroup led
 * @{ */

#ifndef __LED_H__
#define __LED_H__

/******************************************************************************
Preprocessor Directives
*******************************************************************************/

#include "GPIO.h"

#include "NewAssert.h"

#include <stdbool.h>
#include <stdint.h>

#define LED_POOL_SIZE 3

/******************************************************************************
Initialization
*******************************************************************************/

typedef struct Led_t Led_t;

/**
 * @brief                       Initialize a light-emitting diode (LED) as an `Led_t`.
 *
 * @param[in] gpioPort_ptr      Pointer to a `struct` representing a GPIO port.
 * @param[in] pin               GPIO pin to use.
 * @param[out] Led_t*           Pointer to LED data structure.
 */
Led_t * Led_Init(GPIO_Port_t * gpioPort_ptr, GPIO_Pin_t pin);

/******************************************************************************
Configuration
*******************************************************************************/

/**
 * @brief                       Get the GPIO port associated with the LED.
 *
 * @param[in] led_ptr           Pointer to LED data structure.
 * @param[out] GPIO_Port_t*     Pointer to a GPIO port data structure.
 */
GPIO_Port_t * Led_GetPort(Led_t * led_ptr);

/**
 * @brief                       Get the GPIO pin associated with the LED.
 *
 * @param[in] led_ptr           Pointer to LED data structure.
 * @param[out] GPIO_Pin_t       GPIO pin associated with the LED.
 */
GPIO_Pin_t LED_GetPin(Led_t * led_ptr);

/**
 * @brief                       Check the LED's status.
 *
 * @param[in] led_ptr           Pointer to LED data structure.
 * @param[out] true             the LED is `ON`.
 * @param[out] false            the LED is `OFF`.
 */
bool Led_isOn(Led_t * led_ptr);

/******************************************************************************
Operations
*******************************************************************************/

/**
 * @brief                       Turn the LED `ON`.
 * @param[in] led_ptr           Pointer to LED data structure.
 */
void Led_TurnOn(Led_t * led_ptr);

/**
 * @brief                       Turn the LED `OFF`.
 * @param[in] led_ptr           Pointer to LED data structure.
 */
void Led_TurnOff(Led_t * led_ptr);

/**
 * @brief                       Toggle the LED (i.e. `OFF` -> `ON` or `ON` -> `OFF`).
 * @param[in] led_ptr           Pointer to LED data structure.
 */
void Led_Toggle(Led_t * led_ptr);

#endif               // __LED_H__

/// @}
