/** @addtogroup led
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for LED module.
 * @ingroup led
 */

#include "Led.h"

/******************************************************************************
SECTIONS
        Initialization
        Status Checking
        Operations
*******************************************************************************/

#include "GPIO.h"

#include "NewAssert.h"

#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
Initialization
*******************************************************************************/

typedef struct LedStruct_t {
    GpioPort_t GPIO_PORT_PTR;               ///< pointer to GPIO port data structure
    GpioPin_t GPIO_PIN;                     ///< GPIO pin number
    volatile uint32_t * gpioDataRegister;
    bool isOn;                              ///< state indicator
    bool isInit;
} LedStruct_t;

static LedStruct_t Led_ObjPool[LED_POOL_SIZE] = { 0 };
static uint8_t num_free_leds = LED_POOL_SIZE;

Led_t Led_Init(GpioPort_t gpioPort, GpioPin_t pin) {
    Assert(GPIO_isPortInit(gpioPort));
    Assert(num_free_leds > 0);

    // Initialize GPIO port pin
    GPIO_ConfigDirOutput(gpioPort, pin);
    GPIO_ConfigPullDown(gpioPort, pin);
    GPIO_EnableDigital(gpioPort, pin);
    GPIO_WriteLow(gpioPort, pin);

    // Initialize LED struct
    num_free_leds -= 1;
    Led_t led = &Led_ObjPool[num_free_leds];

    led->GPIO_PORT_PTR = gpioPort;
    led->GPIO_PIN = pin;
    led->gpioDataRegister = GPIO_getDataRegister(gpioPort);
    led->isOn = false;
    led->isInit = true;

    return led;
}

GpioPort_t Led_GetPort(Led_t led) {
    Assert(led->isInit);
    return led->GPIO_PORT_PTR;
}

GpioPin_t Led_GetPin(Led_t led) {
    Assert(led->isInit);
    return led->GPIO_PIN;
}

/******************************************************************************
Status Checking
*******************************************************************************/

bool Led_isInit(Led_t led) {
    return led->isInit;
}

bool Led_isOn(Led_t led) {
    Assert(led->isInit);
    return led->isOn;
}

/******************************************************************************
Operations
*******************************************************************************/

void Led_TurnOn(Led_t led) {
    Assert(led->isInit);
    *led->gpioDataRegister |= led->GPIO_PIN;
    led->isOn = true;
    return;
}

void Led_TurnOff(Led_t led) {
    Assert(led->isInit);
    *led->gpioDataRegister &= ~(led->GPIO_PIN);
    led->isOn = false;
    return;
}

void Led_Toggle(Led_t led) {
    Assert(led->isInit);
    *led->gpioDataRegister ^= led->GPIO_PIN;
    led->isOn = !led->isOn;
    return;
}

/// @}
