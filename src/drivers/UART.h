/**
 * @file        UART.h
 * @author      Bryan McElvy
 * @brief       Driver module for serial communication via UART0 and UART 1.
 * 
 *              UART0 uses PA0 and PA1, which are not broken out but do connect
 *              to a PC's serial port via USB.
 * 
 *              UART1 uses PB0 (Rx) and PB1 (Tx), which are not broken out but
 *              do not connect to a serial port.
 * 
 * @addtogroup  uart
 * @{
 */

#ifndef __UART_H___
#define __UART_H___

#include "tm4c123gh6pm.h"

/**********************************************************************
UART0
***********************************************************************/

/**
 * @brief   Initialize UART0 to a baud rate of 115200, 
 *          8-bit data length, 1 start bit, and 1 stop bit.
 */
void UART0_Init(void);

/**
 * @brief   Read a single character from UART0.
 * 
 * @return  input_char
 */
unsigned char UART0_ReadChar(void);

/**
 * @brief   Write a single character to UART0.
 * 
 * @param   input_char
 */
void UART0_WriteChar(unsigned char input_char);

/**
 * @brief   Write a C string to UART0.
 * 
 * @param   str_ptr pointer to C string
 */
void UART0_WriteStr(unsigned char * str_ptr);

/**********************************************************************
UART1
***********************************************************************/

/**
 * @brief   Initialize UART1 to a baud rate of 115200, 
 *          8-bit data length, 1 start bit, and 1 stop bit.
 */
void UART1_Init(void);

/**
 * @brief   Read a single character from UART1.
 * 
 * @return  input_char
 */
unsigned char UART1_ReadChar(void);

/**
 * @brief   Write a single character to UART1.
 * 
 * @param   input_char
 */
void UART1_WriteChar(unsigned char input_char);

/**
 * @brief   Write a C string to UART1.
 * 
 * @param   str_ptr pointer to C string
 */
void UART1_WriteStr(unsigned char * str_ptr);


#endif

/** @} */
