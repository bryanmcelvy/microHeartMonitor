/**
 * @file
 * @author  Bryan McElvy
 * @brief   Functions to output debugging information to a serial port via UART.
 */

#ifndef DEBUG_H
#define DEBUG_H

#include "UART.h"

#include "NewAssert.h"

#include "tm4c123gh6pm.h"
#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
Initialization
*******************************************************************************/

/// @brief              Init. the Debug module and send a start message to the port.
void Debug_Init(void);

/******************************************************************************
Serial Output
*******************************************************************************/

/**
 * @brief               Send a message to the serial port.
 *
 * @param message       (Pointer to) array of ASCII characters.
 */
void Debug_SendMsg(void * message);

typedef enum { START_MSG, DAQ_INIT, QRS_INIT, LCD_INIT, ASSERT_FALSE } msg_t;

/**
 * @brief               Send a message from the message list.
 *
 * @param[in] msg       Message to send.
 */
void Debug_SendFromList(msg_t msg);

/**
 * @brief               Write a floating-point value to the serial port.
 *
 * @param[in] value     Floating-point value.
 */
void Debug_WriteFloat(double value);

/******************************************************************************
Assertions
*******************************************************************************/

/**
 * @brief               Stops program if `condition` is `true`.
 *                      Useful for bug detection during debugging.
 *
 * @param condition
 */
void Debug_Assert(bool condition);

#endif               // DEBUG_H
