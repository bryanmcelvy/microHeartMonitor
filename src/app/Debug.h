/**
 * @file
 * @author  Bryan McElvy
 * @brief   Functions to output debugging information to a serial port via UART.
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "UART.h"

#include "tm4c123gh6pm.h"

enum messages { START_MSG, DAQ_INIT, QRS_INIT, LCD_INIT };

/******************************************************************************
[SECTION NAME]
*******************************************************************************/

/**
 * @brief           Initialize the Debug module and send a start message to the
 *                  serial port.
 *
 */
void Debug_Init(void);

/**
 * @brief           Send a message to the serial port.
 *
 * @param message   (Pointer to) array of ASCII characters.
 */
void Debug_SendMsg(void * message);

void Debug_SendFromList(uint8_t msg_idx);

#endif
