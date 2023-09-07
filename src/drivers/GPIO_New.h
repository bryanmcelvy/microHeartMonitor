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
 * @brief                   Initialize a GPIO Port and return a pointer to its `struct`.
 *
 * @param[in] portName          Name of the chosen port.
 * @return GPIO_Port_t*     Pointer to the GPIO port's `struct`.
 */
GPIO_Port_t * GPIO_InitPort(GPIO_PortName_t portName);

/**
 * @brief                   Check if the GPIO port is initialized.
 *
 * @param[in] gpioPort      Pointer to the specified GPIO port.
 * @param[out] true         The GPIO port is initialized.
 * @param[out] false        The GPIO port has not been initialized.
 */
bool GPIO_isPortInit(GPIO_Port_t * gpioPort);

/******************************************************************************
Configuration (Digital I/O)
*******************************************************************************/

/**
 * @brief           Configure the direction of the specified GPIO pins. All pins
 *                  are configured to `INPUT` by default, so this function should
 *                  only be called to specify `OUTPUT` pins.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] bitMask   Bit mask corresponding to the intended `OUTPUT` pin(s).
 */
void GPIO_ConfigDirOutput(GPIO_Port_t * gpioPort, uint8_t bitMask);

/**
 * @brief           Configure the specified GPIO pins as `INPUT` pins. All pins
 *                  are configured to `INPUT` by default, so this function is
 *                  technically unnecessary, but useful for code readability.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] bitMask   Bit mask corresponding to the intended `INPUT` pin(s).
 */
void GPIO_ConfigDirInput(GPIO_Port_t * gpioPort, uint8_t bitMask);

/**
 * @brief           Activate the specified pins' internal pull-up resistors.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] bitMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_ConfigPullUp(GPIO_Port_t * gpioPort, uint8_t bitMask);

/**
 * @brief           Activate the specified pins' internal pull-down resistors.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] bitMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_ConfigPullDown(GPIO_Port_t * gpioPort, uint8_t bitMask);

/**
 * @brief           Configure the specified pins' drive strength. Pins are
 *                  initialized with 2[mA] drive strength, so this is only
 *                  needed for a drive strength of 4[mA] or 8[mA].
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] bitMask   Bit mask corresponding to the intended pin(s).
 * @param[in] drive_mA  Drive strength in [mA]. Should be 2, 4, or 8 [mA].
 */
void GPIO_ConfigDriveSelect(GPIO_Port_t * gpioPort, uint8_t bitMask, uint8_t drive_mA);

/**
 * @brief           Enable digital I/O for the specified pins.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] bitMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_EnableDigital(GPIO_Port_t * gpioPort, uint8_t bitMask);

/**
 * @brief           Disable digital I/O for the specified pins.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] bitMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_DisableDigital(GPIO_Port_t * gpioPort, uint8_t bitMask);

/******************************************************************************
Basic Functions (Digital I/O)
*******************************************************************************/

/**
 * @brief           Read from the specified GPIO pin.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] bitMask   Bit mask corresponding to the intended pin(s).
 */
uint8_t GPIO_ReadPins(GPIO_Port_t * gpioPort, uint8_t bitMask);

/**
 * @brief           Write a `1` to the specified GPIO pins.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] bitMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_WriteHigh(GPIO_Port_t * gpioPort, uint8_t bitMask);

/**
 * @brief           Write a `0` to the specified GPIO pins.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] bitMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_WriteLow(GPIO_Port_t * gpioPort, uint8_t bitMask);

/**
 * @brief           Toggle the specified GPIO pins.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] bitMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_Toggle(GPIO_Port_t * gpioPort, uint8_t bitMask);

/******************************************************************************
Configuration (Alternate/Analog Modes)
*******************************************************************************/

// TODO: Write header comment
void GPIO_ConfigAltMode(GPIO_Port_t * gpioPort, uint8_t bitMask);

// TODO: Write header comment
void GPIO_ConfigPortCtrl(GPIO_Port_t * gpioPort, uint8_t bitMask, uint8_t fieldEncoding);

// TODO: Write header comment
void GPIO_ConfigAnalog(GPIO_Port_t * gpioPort, uint8_t bitMask);

#endif               // __DEBUG_H__
