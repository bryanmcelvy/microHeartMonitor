/**
 * @addtogroup gpio
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Header file for general-purpose input/output (GPIO) device driver.
 */

#ifndef GPIO_H
#define GPIO_H

#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
Type Definitions + Initialization
*******************************************************************************/
typedef enum {
    GPIO_PORT_A,
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D,
    GPIO_PORT_E,
    GPIO_PORT_F,
    A = GPIO_PORT_A,
    B = GPIO_PORT_B,
    C = GPIO_PORT_C,
    D = GPIO_PORT_D,
    E = GPIO_PORT_E,
    F = GPIO_PORT_F
} GPIO_PortName_t;

typedef struct GpioPortStruct_t * GpioPort_t;

/**
 * @brief                   Initialize a GPIO Port and return a pointer to its `struct`.
 *
 * @param[in] portName      Name of the chosen port.
 * @param[out] gpioPort     Pointer to the specified GPIO port.
 */
GpioPort_t GPIO_InitPort(GPIO_PortName_t portName);

/**
 * @brief                   Get the base address of a GPIO port.
 *
 * @param[in] gpioPort      Pointer to the specified GPIO port.
 * @param[out] baseAddress  Base address of the GPIO port.
 */
uint32_t GPIO_getBaseAddr(GpioPort_t gpioPort);

/**
 * @brief                   Check if the GPIO port is initialized.
 *
 * @param[in] gpioPort      Pointer to the specified GPIO port.
 * @param[out] true         The GPIO port is initialized.
 * @param[out] false        The GPIO port has not been initialized.
 */
bool GPIO_isPortInit(GpioPort_t gpioPort);

/******************************************************************************
Configuration (Digital I/O)
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
} GpioPin_t;               ///< bit masks for each GPIO pin. Can be `OR`ed together for funcs.

enum GPIO_LAUNCHPAD_LEDS {
    LED_RED = GPIO_PIN1,                 ///< PF1
    LED_GREEN = GPIO_PIN3,               ///< PF3
    LED_BLUE = GPIO_PIN2,                ///< PF2

    LED_YELLOW = (LED_RED + LED_GREEN),
    LED_CYAN = (LED_BLUE + LED_GREEN),
    LED_PURPLE = (LED_RED + LED_BLUE),
    LED_WHITE = (LED_RED + LED_BLUE + LED_GREEN)
};

typedef enum {
    GPIO_INPUT,
    GPIO_OUTPUT
} gpioDir_t;

/**
 * @brief               Configure the direction of the specified GPIO pins.
 *
 * @pre                 Initialize the GPIO port.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 * @param[in] direction The direction for the intended pin(s).
 *
 * @post                The specified GPIO pins are now configured as inputs or outputs.
 *
 * @see                 GPIO_InitPort()
 */
void GPIO_configDirection(GpioPort_t gpioPort, GpioPin_t pinMask, gpioDir_t direction);

typedef enum {
    PULLUP,
    PULLDOWN
} gpioResistor_t;

/**
 * @brief               Activate the specified pins' internal pull-up or pull-down resistors.
 *
 * @pre                 Initialize the GPIO port.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 * @param[in] resistor  The type of resistor to use.
 *
 * @post                The pull-up/pull-down resistor(s) are now activated.
 *
 * @see                 GPIO_InitPort()
 */
void GPIO_configResistor(GpioPort_t gpioPort, GpioPin_t pinMask, gpioResistor_t resistor);

/**
 * @brief               Configure the specified pins' drive strength. Pins are
 *                      initialized with 2[mA] drive strength, so this is only
 *                      needed for a drive strength of 4[mA] or 8[mA].
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 * @param[in] drive_mA  Drive strength in [mA]. Should be 2, 4, or 8 [mA].
 */
void GPIO_ConfigDriveStrength(GpioPort_t gpioPort, GpioPin_t pinMask, uint8_t drive_mA);

/**
 * @brief               Enable digital I/O for the specified pins.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_EnableDigital(GpioPort_t gpioPort, GpioPin_t pinMask);

/**
 * @brief               Disable digital I/O for the specified pins.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_DisableDigital(GpioPort_t gpioPort, GpioPin_t pinMask);

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
void GPIO_ConfigInterrupts_Edge(GpioPort_t gpioPort, GpioPin_t pinMask, bool risingEdge);

/**
 * @brief                   Configure the specified GPIO pins to trigger an
 *                          interrupt on both edges of an input.
 *
 * @param[in] gpioPort      Pointer to the specified GPIO port.
 * @param[in] pinMask       Bit mask corresponding to the intended pin(s).
 */
void GPIO_ConfigInterrupts_BothEdges(GpioPort_t gpioPort, GpioPin_t pinMask);

/**
 * @brief                   Configure the specified GPIO pins to trigger an
 *                          interrupt on a high level or low level pulse.
 *
 * @param[in] gpioPort      Pointer to the specified GPIO port.
 * @param[in] pinMask       Bit mask corresponding to the intended pin(s).
 * @param[in] highLevel     `true` for high level, `false` for low level
 */
void GPIO_ConfigInterrupts_LevelTrig(GpioPort_t gpioPort, GpioPin_t pinMask, bool highLevel);

/**
 * @brief                   Configure interrupts for the selected port in the NVIC.
 *
 * @param[in] gpioPort      Pointer to the specified GPIO port.
 * @param[in] priority      Priority number between 0 (highest) and 7 (lowest).
 */
void GPIO_ConfigNVIC(GpioPort_t gpioPort, uint8_t priority);

/******************************************************************************
Basic Functions (Digital I/O)
*******************************************************************************/

/**
 * @brief                   Get the address of a GPIO port's data register.
 *
 * @param[in] gpioPort      Pointer to the specified GPIO port.
 * @param[out] dataRegister Address of the GPIO port's data register.
 */
volatile uint32_t * GPIO_getDataRegister(GpioPort_t gpioPort);

/**
 * @brief               Read from the specified GPIO pin.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 */
uint8_t GPIO_ReadPins(GpioPort_t gpioPort, GpioPin_t pinMask);

/**
 * @brief               Write a `1` to the specified GPIO pins.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_WriteHigh(GpioPort_t gpioPort, GpioPin_t pinMask);

/**
 * @brief               Write a `0` to the specified GPIO pins.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_WriteLow(GpioPort_t gpioPort, GpioPin_t pinMask);

/**
 * @brief               Toggle the specified GPIO pins.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_Toggle(GpioPort_t gpioPort, GpioPin_t pinMask);

/******************************************************************************
Configuration (Alternate/Analog Modes)
*******************************************************************************/

/**
 * @brief               Activate the alternate mode for the specified pins.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_ConfigAltMode(GpioPort_t gpioPort, GpioPin_t pinMask);

/**
 * @brief               Specify the alternate mode to use for the specified pins.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 * @param[in] fieldEncoding Number corresponding to intended alternate mode.
 */
void GPIO_ConfigPortCtrl(GpioPort_t gpioPort, GpioPin_t pinMask, uint8_t fieldEncoding);

/**
 * @brief               Activate analog mode for the specified GPIO pins.
 *
 * @param[in] gpioPort  Pointer to the specified GPIO port.
 * @param[in] pinMask   Bit mask corresponding to the intended pin(s).
 */
void GPIO_ConfigAnalog(GpioPort_t gpioPort, GpioPin_t pinMask);

#endif                  // GPIO_H

/** @} */               // gpio
