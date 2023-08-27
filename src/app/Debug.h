/**
 * @file
 * @author  Bryan McElvy
 * @brief   Functions to output debugging information to a serial port via UART.
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "UART.h"

#include "arm_math_types.h"
#include "tm4c123gh6pm.h"
#include <stdbool.h>
#include <stdint.h>

enum messages { START_MSG, DAQ_INIT, QRS_INIT, LCD_INIT, ASSERT_FALSE };

/******************************************************************************
Initialization
*******************************************************************************/

/**
 * @brief           Initialize the Debug module and send a start message to the
 *                  serial port.
 *
 */
void Debug_Init(void);

/******************************************************************************
Serial Output
*******************************************************************************/

/**
 * @brief           Send a message to the serial port.
 *
 * @param message   (Pointer to) array of ASCII characters.
 */
void Debug_SendMsg(void * message);

void Debug_SendFromList(uint8_t msg_idx);

void Debug_WriteFloat(float64_t value);

/******************************************************************************
Assertions
*******************************************************************************/

/**
 * @brief           Stops program if `condition` is `true`.
 *                  Useful for bug detection during debugging.
 *
 * @param condition
 */
void Debug_Assert(bool condition);

#endif
