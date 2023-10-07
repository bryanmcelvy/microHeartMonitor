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
        Configuration
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
    GPIO_Port_t * GPIO_PORT_PTR;               ///< pointer to GPIO port data structure
    GPIO_Pin_t GPIO_PIN;                       ///< GPIO pin number
    bool is_ON;                                ///< state indicator
    bool isInit;
} LedStruct_t;

static LedStruct_t Led_ObjPool[LED_POOL_SIZE] = { 0 };
static uint8_t num_free_leds = LED_POOL_SIZE;

Led_t Led_Init(GPIO_Port_t * gpioPort, GPIO_Pin_t pin) {
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
    led->is_ON = false;
    led->isInit = true;

    return led;
}

bool Led_isInit(Led_t led) {
    return led->isInit;
}

/******************************************************************************
Configuration
*******************************************************************************/

GPIO_Port_t * Led_GetPort(Led_t led) {
    Assert(led->isInit);
    return led->GPIO_PORT_PTR;
}

GPIO_Pin_t Led_GetPin(Led_t led) {
    Assert(led->isInit);
    return led->GPIO_PIN;
}

bool Led_isOn(Led_t led) {
    Assert(led->isInit);
    return led->is_ON;
}

/******************************************************************************
Operations
*******************************************************************************/

void Led_TurnOn(Led_t led) {
    Assert(led->isInit);
    GPIO_WriteHigh(led->GPIO_PORT_PTR, led->GPIO_PIN);
    led->is_ON = true;
    return;
}

void Led_TurnOff(Led_t led) {
    Assert(led->isInit);
    GPIO_WriteLow(led->GPIO_PORT_PTR, led->GPIO_PIN);
    led->is_ON = false;
    return;
}

void Led_Toggle(Led_t led) {
    Assert(led->isInit);
    GPIO_Toggle(led->GPIO_PORT_PTR, led->GPIO_PIN);
    led->is_ON = !led->is_ON;
    return;
}

/// @}
