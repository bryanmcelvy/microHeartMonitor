/**
 * @addtogroup  gpio
 * @{
 * 
 * @file        GPIO.h
 * @author      Bryan McElvy
 * @brief       Driver module for using the LaunchPad's onboard switches and RGB LEDs for GPIO and interrupts.
 * 
 */

#ifndef __GPIO_H___
#define __GPIO_H___

#include "tm4c123gh6pm.h"

#include <stdint.h>

// Bitmasks
#define LED_RED         (uint8_t) 0x02
#define LED_GREEN       (uint8_t) 0x08
#define LED_BLUE        (uint8_t) 0x04

#define LED_YELLOW      (LED_RED + LED_GREEN)
#define LED_CYAN        (LED_BLUE + LED_GREEN)
#define LED_PURPLE      (LED_RED + LED_BLUE)
#define LED_WHITE       (LED_RED + LED_BLUE + LED_GREEN)

/**
 * @brief   Initialize GPIO Port F.
 * 
 */
void GPIO_PF_Init(void);

/**
 * @brief   Initialize PF1-3 to interface the LaunchPad's onboard RGB LED.
 * 
 */
void GPIO_PF_LED_Init(void);

/**
 * @brief   Write a 1 or 0 to the selected LED(s).
 * @param   color_mask  Hex. number of LED pin(s) to write to.
 *                      0x02 (PF1) – RED;
 *                      0x04 (PF2) – BLUE;
 *                      0x08 (PF3) – GREEN
 * @param   on_or_off   =0 for OFF, >=1 for ON
 */
void GPIO_PF_LED_Write(uint8_t color_mask, uint8_t on_or_off);

/**
 * @brief   Toggle the selected LED(s).
 * @param   color_mask  Hex. number of LED pin(s) to write to.
 *                      0x02 (PF1) – RED;
 *                      0x04 (PF2) – BLUE;
 *                      0x08 (PF3) – GREEN
 */
void GPIO_PF_LED_Toggle(uint8_t color_mask);

/**
 * @brief   Initialize PF0/4 to interface the LaunchPad's onboard switches.
 *          PF4 is Sw1, and PF0 is Sw2.
 * 
 */
void GPIO_PF_Sw_Init(void);

/**
 * @brief   Initialize GPIO Port F interrupts via Sw1 and Sw2.
 * 
 */
void GPIO_PF_Interrupt_Init(void);

#endif

/** @} */
