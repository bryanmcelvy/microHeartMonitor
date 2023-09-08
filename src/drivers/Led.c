/**
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for LED module.
 * @ingroup led
 */

/** @addtogroup led
 * @{ */

/******************************************************************************
SECTIONS
        Preprocessor Directives
        Initialization
        Configuration
        Operations
*******************************************************************************/

/******************************************************************************
Preprocessor Directives
*******************************************************************************/

#include "Led.h"

#include "GPIO.h"

#include "NewAssert.h"

#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
Initialization
*******************************************************************************/

struct Led_t {
    GPIO_Port_t * GPIO_PORT_PTR;               ///< pointer to GPIO port data structure
    GPIO_Pin_t GPIO_PIN;                       ///< GPIO pin number
    bool is_ON;                                ///< state indicator
};

static Led_t Led_ObjPool[LED_POOL_SIZE] = { 0 };
static uint8_t num_free_leds = LED_POOL_SIZE;

Led_t * Led_Init(GPIO_Port_t * gpioPort_ptr, GPIO_Pin_t pin) {

    // Initialize GPIO port pin
    Assert(GPIO_isPortInit(gpioPort_ptr));
    GPIO_ConfigDirOutput(gpioPort_ptr, pin);
    GPIO_ConfigPullDown(gpioPort_ptr, pin);
    GPIO_EnableDigital(gpioPort_ptr, pin);

    // Initialize LED struct
    Assert(num_free_leds > 0);
    num_free_leds -= 1;

    Led_t * led_ptr = &Led_ObjPool[num_free_leds];
    led_ptr->GPIO_PORT_PTR = gpioPort_ptr;
    led_ptr->GPIO_PIN = pin;
    led_ptr->is_ON = false;

    return led_ptr;
}

/******************************************************************************
Configuration
*******************************************************************************/

GPIO_Port_t * Led_GetPort(Led_t * led_ptr) {
    return led_ptr->GPIO_PORT_PTR;
}

GPIO_Pin_t LED_GetPin(Led_t * led_ptr) {
    return led_ptr->GPIO_PIN;
}

bool Led_isOn(Led_t * led_ptr) {
    return led_ptr->is_ON;
}

/******************************************************************************
Operations
*******************************************************************************/

void Led_TurnOn(Led_t * led_ptr) {
    GPIO_WriteHigh(led_ptr->GPIO_PORT_PTR, led_ptr->GPIO_PIN);
    led_ptr->is_ON = true;
    return;
}

void Led_TurnOff(Led_t * led_ptr) {
    GPIO_WriteLow(led_ptr->GPIO_PORT_PTR, led_ptr->GPIO_PIN);
    led_ptr->is_ON = false;
    return;
}

void Led_Toggle(Led_t * led_ptr) {
    GPIO_Toggle(led_ptr->GPIO_PORT_PTR, led_ptr->GPIO_PIN);
    led_ptr->is_ON = !led_ptr->is_ON;
    return;
}

/// @}
