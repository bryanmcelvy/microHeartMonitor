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

#ifndef UART_H
#define UART_H

/******************************************************************************
SECTIONS
        Initialization
        Reading
        Writing
*******************************************************************************/

#include "GPIO.h"

#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
Initialization
*******************************************************************************/

typedef struct UartStruct_t * Uart_t;

typedef enum {
    UART0,
    UART1,
    UART2,
    UART3,
    UART4,
    UART5,
    UART6,
    UART7
} uartNum_t;

/**
 * @brief                       Initialize the specified UART peripheral.
 *
 * @param[in] port              GPIO port to use.
 * @param[in] uartNum           UART number. Should be either one of the
                                enumerated constants or an int in range [0, 7].
 * @param[out] uart             (Pointer to) initialized UART peripheral.
 */
Uart_t UART_Init(GpioPort_t port, uartNum_t uartNum);

/**
 * @brief                       Check if the UART object is initialized.
 *
 * @param[in] uart              UART to check.
 * @param[out] true             The UART object is initialized.
 * @param[out] false            The UART object is not initialized.
 */
bool UART_isInit(Uart_t uart);

/******************************************************************************
Reading
*******************************************************************************/

/**
 * @brief                       Read a single ASCII character from the UART.
 *
 * @param[in] uart              UART to read from.
 * @param[out] unsigned char    ASCII character from sender.
 */
unsigned char UART_ReadChar(Uart_t uart);

/******************************************************************************
Writing
*******************************************************************************/

/**
 * @brief                       Write a single character to the UART.
 *
 * @param[in] uart              UART to write to.
 * @param[in] input_char        ASCII character to send.
 */
void UART_WriteChar(Uart_t uart, unsigned char inputChar);

/**
 * @brief                       Write a C string to the UART.
 *
 * @param[in] uart              UART to write to.
 * @param[in] input_str         Array of ASCII characters.
 */
void UART_WriteStr(Uart_t uart, void * inputStr);

/**
 * @brief                       Write a 32-bit unsigned integer the UART.
 *
 * @param[in] uart              UART to write to.
 * @param[in] n                 Unsigned 32-bit `int` to be converted and transmitted.
 */
void UART_WriteInt(Uart_t uart, int32_t n);

/**
 * @brief                       Write a floating-point number the UART.
 *
 * @param[in] uart              UART to write to.
 * @param[in] n                 Floating-point number to be converted and transmitted.
 * @param[in] num_decimals      Number of digits after the decimal point to include.
 */
void UART_WriteFloat(Uart_t uart, float n, uint8_t numDecimals);

#endif

/** @} */
