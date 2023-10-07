/**
 * @addtogroup led
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Interface for LED module.
 */

#ifndef LED_H
#define LED_H

/******************************************************************************
Preprocessor Directives
*******************************************************************************/

#include "GPIO.h"

#include <stdbool.h>

#ifndef LED_POOL_SIZE
#define LED_POOL_SIZE 3
#endif

/******************************************************************************
Initialization
*******************************************************************************/

typedef struct Led_t Led_t;

/**
 * @brief                       Initialize a light-emitting diode (LED) as an `Led_t`.
 *
 * @param[in] gpioPort          Pointer to a `struct` representing a GPIO port.
 * @param[in] pin               GPIO pin to use.
 * @param[out] Led_t*           Pointer to LED data structure.
 */
Led_t * Led_Init(GPIO_Port_t * gpioPort, GPIO_Pin_t pin);

/******************************************************************************
Status Checking
*******************************************************************************/

/**
 * @brief                       Get the GPIO port associated with the LED.
 *
 * @param[in] led               Pointer to LED data structure.
 * @param[out] GPIO_Port_t*     Pointer to a GPIO port data structure.
 */
GPIO_Port_t * Led_GetPort(Led_t * led);

/**
 * @brief                       Get the GPIO pin associated with the LED.
 *
 * @param[in] led               Pointer to LED data structure.
 * @param[out] GPIO_Pin_t       GPIO pin associated with the LED.
 */
GPIO_Pin_t Led_GetPin(Led_t * led);

/**
 * @brief                       Check the LED's status.
 *
 * @param[in] led               Pointer to LED data structure.
 * @param[out] true             the LED is `ON`.
 * @param[out] false            the LED is `OFF`.
 */
bool Led_isOn(Led_t * led);

/******************************************************************************
Operations
*******************************************************************************/

/**
 * @brief                       Turn the LED `ON`.
 * @param[in] led               Pointer to LED data structure.
 */
void Led_TurnOn(Led_t * led);

/**
 * @brief                       Turn the LED `OFF`.
 * @param[in] led               Pointer to LED data structure.
 */
void Led_TurnOff(Led_t * led);

/**
 * @brief                       Toggle the LED (i.e. `OFF` -> `ON` or `ON` -> `OFF`).
 * @param[in] led               Pointer to LED data structure.
 */
void Led_Toggle(Led_t * led);

#endif               // LED_H

/// @}
