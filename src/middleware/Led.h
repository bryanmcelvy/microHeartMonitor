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
#define LED_POOL_SIZE 1
#endif

/******************************************************************************
Initialization
*******************************************************************************/

typedef struct LedStruct_t * Led_t;

/** @name Initialization & Configuration */               /// @{

/**
 * @brief                       Initialize a light-emitting diode (LED) as an `Led_t`.
 *
 * @param[in] gpioPort          Pointer to a `struct` representing a GPIO port.
 * @param[in] pin               GPIO pin to use.
 * @param[out] led              Pointer to LED data structure.
 */
Led_t Led_Init(GpioPort_t gpioPort, GpioPin_t pin);

/**
 * @brief                       Get the GPIO port associated with the LED.
 *
 * @pre                         Initialize the LED.
 *
 * @param[in] led               Pointer to LED data structure.
 * @param[out] gpioPort         Pointer to a GPIO port data structure.
 *
 * @see                         Led_Init(), Led_GetPin()
 */
GpioPort_t Led_GetPort(Led_t led);

/**
 * @brief                       Get the GPIO pin associated with the LED.
 *
 * @pre                         Initialize the LED.
 *
 * @param[in] led               Pointer to LED data structure.
 * @param[out] pin              GPIO pin associated with the LED.
 *
 * @see                         Led_Init(), Led_GetPort()
 */
GpioPin_t Led_GetPin(Led_t led);

/** @} */               // Initialization & Configuration

/******************************************************************************
Status Checking
*******************************************************************************/

/** @name Status Checking */               /// @{

/**
 * @brief                       Check if an LED is initialized.
 *
 * @param[in] led               Pointer to LED data structure.
 * @param[out] true             The LED is initialized.
 * @param[out] false            The LED is not initialized.
 *
 * @see                         Led_Init()
 */
bool Led_isInit(Led_t led);

/**
 * @brief                       Check the LED's status.
 *
 * @pre                         Initialize the LED.
 *
 * @param[in] led               Pointer to LED data structure.
 * @param[out] true             the LED is `ON`.
 * @param[out] false            the LED is `OFF`.
 *
 * @see                         Led_TurnOn(), Led_TurnOff(), Led_Toggle()
 */
bool Led_isOn(Led_t led);

/** @} */               // Status Checking

/******************************************************************************
Operations
*******************************************************************************/

/** @name Operations */               /// @{

/**
 * @brief                       Turn an LED `ON`.
 *
 * @pre                         Initialize the LED.
 *
 * @param[in] led               Pointer to LED data structure.
 * @post                        The LED is turned ON.
 *
 * @see                         Led_TurnOff(), Led_Toggle()
 */
void Led_TurnOn(Led_t led);

/**
 * @brief                       Turn an LED `OFF`.
 *
 * @pre                         Initialize the LED.
 *
 * @param[in] led               Pointer to LED data structure.
 * @post                        The LED is turned OFF.
 *
 * @see                         Led_TurnOn(), Led_Toggle()
 */
void Led_TurnOff(Led_t led);

/**
 * @brief                       Toggle an LED.
 *
 * @pre                         Initialize the LED.
 *
 * @param[in] led               Pointer to LED data structure.
 * @post                        The LED's state is flipped (i.e. ON -> OFF or OFF -> ON).
 *
 * @see                         Led_TurnOn(), Led_TurnOff()
 */
void Led_Toggle(Led_t led);

/** @} */               // Operations

#endif                  // LED_H

/// @}
