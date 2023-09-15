/**
 * @addtogroup  uart
 * @{
 *
 * @file        UART.h
 * @author      Bryan McElvy
 * @brief       Driver module for serial communication via UART0 and UART 1.
 *
 *              UART0 uses PA0 and PA1, which are not broken out but can connect
 *              to a PC's serial port via USB.
 *
 *              UART1 uses PB0 (Rx) and PB1 (Tx), which are broken out but
 *              do not connect to a serial port.
 */

#ifndef __UART_H___
#define __UART_H___

#include "GPIO.h"

#include "NewAssert.h"

#include "tm4c123gh6pm.h"
#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
Initialization
*******************************************************************************/

typedef struct UART_t UART_t;

typedef enum { UART0, UART1, UART2, UART3, UART4, UART5, UART6, UART7 } UART_Num_t;

UART_t * UART_Init(GPIO_Port_t * port, UART_Num_t uartNum);

/******************************************************************************
Reading
*******************************************************************************/

/**
 * @brief                       Read a single ASCII character from the UART.
 *
 * @param[in] uart              UART to read from.
 * @param[out] unsigned char    ASCII character from sender.
 */
unsigned char UART_ReadChar(UART_t * uart);

/******************************************************************************
Writing
*******************************************************************************/

/**
 * @brief                       Write a single character to the UART.
 *
 * @param[in] uart              UART to read from.
 * @param[in] input_char        ASCII character to send.
 */
void UART_WriteChar(UART_t * uart, unsigned char input_char);

/**
 * @brief                       Write a C string to the UART.
 *
 * @param[in] uart              UART to read from.
 * @param[in] input_str         Array of ASCII characters.
 */
void UART_WriteStr(UART_t * uart, void * input_str);

/**
 * @brief                       Write a 32-bit unsigned integer the UART.
 *
 * @param[in] uart              UART to read from.
 * @param[in] n                 Unsigned 32-bit `int` to be converted and transmitted.
 */
void UART_WriteInt(UART_t * uart, uint32_t n);

/**
 * @brief                       Write a floating-point number the UART.
 *
 * @param[in] uart              UART to read from.
 * @param[in] n                 Floating-point number to be converted and transmitted.
 * @param[in] num_decimals      Number of digits after the decimal point to include.
 */
void UART_WriteFloat(UART_t * uart, double n, uint8_t num_decimals);

#endif

/** @} */
