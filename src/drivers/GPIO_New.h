#ifndef __DEBUG_H__
#define __DEBUG_H__

/******************************************************************************
Preprocessor Directives
*******************************************************************************/

// Includes
#include "tm4c123gh6pm.h"

#include <NewAssert.h>
#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
Type Definitions + Initialization
*******************************************************************************/
typedef enum {
    GPIO_PIN0 = ((uint8_t) 1),
    GPIO_PIN1 = ((uint8_t) (1 << 1)),
    GPIO_PIN2 = ((uint8_t) (1 << 2)),
    GPIO_PIN3 = ((uint8_t) (1 << 3)),
    GPIO_PIN4 = ((uint8_t) (1 << 4)),
    GPIO_PIN5 = ((uint8_t) (1 << 5)),
    GPIO_PIN6 = ((uint8_t) (1 << 6)),
    GPIO_PIN7 = ((uint8_t) (1 << 7)),
    GPIO_ALL_PINS = ((uint8_t) (0xFF))
} GPIO_Pin_t;               ///< bit masks for each GPIO pin. Can be `OR`ed together for funcs.

typedef enum { A, B, C, D, E, F } GPIO_PortName_t;

typedef struct GPIO_Port_t GPIO_Port_t;

/**
 * @brief           Initialize a GPIO Port and return a pointer to its `struct`.
 *
 * @param portName  Name of the chosen port. Should come from the `PORT_NAMES` enum.
 * @return GPIO_Port_t*  Pointer to the GPIO port's `struct`.
 */
GPIO_Port_t * GPIO_PortInit(GPIO_PortName_t portName);

bool GPIO_isPortInit(GPIO_Port_t * gpioPort);

/******************************************************************************
Configuration
*******************************************************************************/

/**
 * @brief           Configure the direction of the specified GPIO pins. All pins
 *                  are configured to `INPUT` by default, so this function should
 *                  only be called to specify `OUTPUT` pins.
 *
 * @param gpioPort  Pointer to the specified GPIO port.
 * @param bitMask   Bit mask corresponding to the intended `OUTPUT` pin(s).
 */
void GPIO_ConfigDir(GPIO_Port_t * gpioPort, uint8_t bitMask);

/**
 * @brief           Activate the specified pins' internal pull-up resistors.
 *
 * @param gpioPort  Pointer to the specified GPIO port.
 * @param bitMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_ConfigPullUp(GPIO_Port_t * gpioPort, uint8_t bitMask);

/**
 * @brief           Activate the specified pins' internal pull-down resistors.
 *
 * @param gpioPort  Pointer to the specified GPIO port.
 * @param bitMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_ConfigPullDown(GPIO_Port_t * gpioPort, uint8_t bitMask);

/**
 * @brief           Configure the specified pins' drive strength. Pins are
 *                  initialized with 2[mA] drive strength, so this is only
 *                  for a drive strength of 4[mA] or 8[mA].
 *
 * @param gpioPort  Pointer to the specified GPIO port.
 * @param bitMask   Bit mask corresponding to the intended pin(s).
 * @param drive_mA  Drive strength in [mA]. Should be 2, 4, or 8 [mA].
 */
void GPIO_ConfigDriveSelect(GPIO_Port_t * gpioPort, uint8_t bitMask, uint8_t drive_mA);

/**
 * @brief           Enable digital I/O for the specified pins.
 *
 * @param gpioPort  Pointer to the specified GPIO port.
 * @param bitMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_EnableDigital(GPIO_Port_t * gpioPort, uint8_t bitMask);

/******************************************************************************
Digital I/O
 *******************************************************************************/

/**
 * @brief           Read from the specified GPIO pin.
 *
 * @param gpioPort  Pointer to the specified GPIO port.
 * @param bitMask   Bit mask corresponding to the intended pin(s).
 */
uint8_t GPIO_ReadPins(GPIO_Port_t * gpioPort, uint8_t bitMask);

/**
 * @brief           Write a `1` to the specified GPIO pins.
 *
 * @param gpioPort  Pointer to the specified GPIO port.
 * @param bitMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_WriteHigh(GPIO_Port_t * gpioPort, uint8_t bitMask);

/**
 * @brief           Write a `0` to the specified GPIO pins.
 *
 * @param gpioPort  Pointer to the specified GPIO port.
 * @param bitMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_WriteLow(GPIO_Port_t * gpioPort, uint8_t bitMask);

/**
 * @brief           Toggle the specified GPIO pins.
 *
 * @param gpioPort  Pointer to the specified GPIO port.
 * @param bitMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_Toggle(GPIO_Port_t * gpioPort, uint8_t bitMask);

#endif               // __DEBUG_H__
