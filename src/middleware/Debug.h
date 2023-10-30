/**
 * @addtogroup debug
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Header file for Debug module.
 */

#ifndef DEBUG_H
#define DEBUG_H

#include "UART.h"

#include <stdbool.h>

/*******************************************************************************
SECTIONS
        Initialization
        Serial Output
        Assertions
********************************************************************************/

/******************************************************************************
Initialization
*******************************************************************************/
/** @name Initialization */               /// @{

/**
 * @brief               Initialize the Debug module.
 *
 * @pre                 Initialize the UART.
 * @param[in] uart      UART to use for serial output.
 * @post                An initialization message is sent to the serial port.
 *
 * @see                 UART_Init()
 */
void Debug_Init(Uart_t uart);

/** @} */               // Initialization

/******************************************************************************
Serial Output
*******************************************************************************/

/** @name Serial Output */               /// @{

/**
 * @brief               Send a message to the serial port.
 *
 * @pre                 Initialize the Debug module.
 * @param message       (Pointer to) array of ASCII characters.
 * @post                A floating point value is written to the serial port.
 *
 * @see                 Debug_SendMsg()
 */
void Debug_SendMsg(void * message);

typedef enum {
    DEBUG_DAQ_INIT,
    DEBUG_QRS_INIT,
    DEBUG_LCD_INIT,
    DEBUG_QRS_START
} Msg_t;

/**
 * @brief               Send a message from the message list.
 *
 * @pre                 Initialize the Debug module.
 * @param[in] msg       An entry from the enumeration.
 * @post                The corresponding message is sent to the serial port.
 *
 * @see                 Debug_SendMsg()
 */
void Debug_SendFromList(Msg_t msg);

/**
 * @brief               Write a floating-point value to the serial port.
 *
 * @pre                 Initialize the Debug module.
 * @param[in] value     Floating-point value.
 * @post                A floating point value is written to the serial port.
 *
 * @see                 Debug_SendMsg()
 */
void Debug_WriteFloat(double value);

/** @} */               // Serial Output

/******************************************************************************
Assertions
*******************************************************************************/
/** @name Assertions */               /// @{

/**
 * @brief               Stops program if `condition` is `true`.
 *                      Useful for bug detection during debugging.
 *
 * @pre                 Initialize the Debug module.
 * @param[in] condition Conditional statement to evaluate.
 * @post                If `condition == true`, the program continues normally.
 *                      If `condition == false`, a message is sent and a breakpoint
 *                      is activated.
 */
void Debug_Assert(bool condition);

/** @} */               // Assertions

#endif                  // DEBUG_H

/** @} */               // debug
