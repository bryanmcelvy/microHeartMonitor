/**
 * @file
 * @author  Bryan McElvy
 * @brief   Header file for general-purpose input/output (GPIO) device driver.
 */

#ifndef GPIO_H
#define GPIO_H

/******************************************************************************
Preprocessor Directives
*******************************************************************************/

// Includes
#include <NewAssert.h>

#include "tm4c123gh6pm.h"
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

enum {
    LED_RED = GPIO_PIN1,
    LED_GREEN = GPIO_PIN3,
    LED_BLUE = GPIO_PIN2,

    LED_YELLOW = (LED_RED + LED_GREEN),
    LED_CYAN = (LED_BLUE + LED_GREEN),
    LED_PURPLE = (LED_RED + LED_BLUE),
    LED_WHITE = (LED_RED + LED_BLUE + LED_GREEN)
};

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
void GPIO_ConfigDirOutput(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask);

/**
 * @brief           Configure the specified GPIO pins as `INPUT` pins. All pins
 *                  are configured to `INPUT` by default, so this function is
 *                  technically unnecessary, but useful for code readability.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] bitMask   Bit mask corresponding to the intended `INPUT` pin(s).
 */
void GPIO_ConfigDirInput(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask);

/**
 * @brief           Activate the specified pins' internal pull-up resistors.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_ConfigPullUp(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask);

/**
 * @brief           Activate the specified pins' internal pull-down resistors.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_ConfigPullDown(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask);

/**
 * @brief           Configure the specified pins' drive strength. Pins are
 *                  initialized with 2[mA] drive strength, so this is only
 *                  needed for a drive strength of 4[mA] or 8[mA].
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 * @param[in] drive_mA  Drive strength in [mA]. Should be 2, 4, or 8 [mA].
 */
void GPIO_ConfigDriveStrength(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask, uint8_t drive_mA);

/**
 * @brief           Enable digital I/O for the specified pins.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_EnableDigital(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask);

/**
 * @brief           Disable digital I/O for the specified pins.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_DisableDigital(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask);

/******************************************************************************
Configuration (Interrupts)
*******************************************************************************/

/**
 * @brief                   Configure the specified GPIO pins to trigger an
 *                          interrupt on the rising or falling edge of an input.
 *
 * @param[in] gpioPort      Pointer to the specified GPIO port.
 * @param[in] pinMask       Bit mask corresponding to the intended pin(s).
 * @param[in] risingEdge    `true` for rising edge, `false` for falling edge
 */
void GPIO_ConfigInterrupts_Edge(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask, bool risingEdge);

/**
 * @brief                   Configure the specified GPIO pins to trigger an
 *                          interrupt on both edges of an input.
 *
 * @param[in] gpioPort      Pointer to the specified GPIO port.
 * @param[in] pinMask       Bit mask corresponding to the intended pin(s).
 */
void GPIO_ConfigInterrupts_BothEdges(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask);

/**
 * @brief                   Configure the specified GPIO pins to trigger an
 *                          interrupt on a high level or low level pulse.
 *
 * @param[in] gpioPort      Pointer to the specified GPIO port.
 * @param[in] pinMask       Bit mask corresponding to the intended pin(s).
 * @param[in] highLevel     `true` for high level, `false` for low level
 */
void GPIO_ConfigInterrupts_LevelTrig(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask, bool highLevel);

/**
 * @brief                   Configure interrupts for the selected port in the NVIC.
 *
 * @param[in] gpioPort      Pointer to the specified GPIO port.
 * @param[in] priority      Priority number between 0 (highest) and 7 (lowest).
 */
void GPIO_ConfigNVIC(GPIO_Port_t * gpioPort, uint8_t priority);

/******************************************************************************
Basic Functions (Digital I/O)
*******************************************************************************/

/**
 * @brief               Read from the specified GPIO pin.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 */
uint8_t GPIO_ReadPins(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask);

/**
 * @brief               Write a `1` to the specified GPIO pins.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_WriteHigh(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask);

/**
 * @brief               Write a `0` to the specified GPIO pins.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_WriteLow(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask);

/**
 * @brief               Toggle the specified GPIO pins.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_Toggle(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask);

/******************************************************************************
Configuration (Alternate/Analog Modes)
*******************************************************************************/

/**
 * @brief               Activate the alternate mode for the specified pins.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_ConfigAltMode(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask);

/**
 * @brief               Specify the alternate mode to use for the specified pins.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 * @param[in] fieldEncoding Number corresponding to intended alternate mode.
 */
void GPIO_ConfigPortCtrl(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask, uint8_t fieldEncoding);

/**
 * @brief               Activate analog mode for the specified GPIO pins.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_ConfigAnalog(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask);

#endif               // GPIO_H
