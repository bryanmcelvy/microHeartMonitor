#ifndef __DEBUG_H__
#define __DEBUG_H__

/******************************************************************************
Preprocessor Directives
*******************************************************************************/
#include "tm4c123gh6pm.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#define GPIO_PIN0 ((uint32_t) 1)
#define GPIO_PIN1 ((uint32_t) (1 << 1))
#define GPIO_PIN2 ((uint32_t) (1 << 2))
#define GPIO_PIN3 ((uint32_t) (1 << 3))
#define GPIO_PIN4 ((uint32_t) (1 << 4))
#define GPIO_PIN5 ((uint32_t) (1 << 5))
#define GPIO_PIN6 ((uint32_t) (1 << 6))
#define GPIO_PIN7 ((uint32_t) (1 << 7))

/******************************************************************************
Type Definitions
*******************************************************************************/
enum port { A, B, C, D, E, F } PORT_NAMES;

typedef struct GPIO_t GPIO_t;

/******************************************************************************
Initialization
*******************************************************************************/

/**
 * @brief           Initialize a GPIO Port and return a pointer to its `struct`.
 *
 * @param portName  Name of the chosen port. Should come from the `PORT_NAMES` enum.
 * @return GPIO_t*  Pointer to the GPIO port's `struct`.
 */
GPIO_t * GPIO_PortInit(uint8_t portName);

/******************************************************************************
Configuration (Digital I/O)
*******************************************************************************/

/**
 * @brief           Configure the direction of the specified GPIO pins. All pins
 *                  are configured to `INPUT` by default, so this function should
 *                  only be called to specify `OUTPUT` pins.
 *
 * @param gpioPort  Name of the chosen port. Should come from the `PORT_NAMES` enum.
 * @param bitMask   Bit mask corresponding to the intended `OUTPUT` pin(s).
 */
void GPIO_ConfigDir(GPIO_t * gpioPort, uint8_t bitMask);

void GPIO_ConfigPullUp(GPIO_t * gpioPort, uint8_t bitMask);

void GPIO_ConfigPullDown(GPIO_t * gpioPort, uint8_t bitMask);

void GPIO_ConfigDriveSelect(GPIO_t * gpioPort, uint8_t bitMask, uint8_t drive_mA);

void GPIO_ConfigEnableDigital(GPIO_t * gpioPort, uint8_t bitMask);

/******************************************************************************
Basic Functions
*******************************************************************************/

void GPIO_SetPins(GPIO_t * gpioPort, uint8_t bitMask);

void GPIO_ClearPins(GPIO_t * gpioPort, uint8_t bitMask);

#endif               // __DEBUG_H__
