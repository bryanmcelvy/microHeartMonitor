/**
 * @file        UART.h
 * @author      Bryan McElvy
 * @brief       Driver module for UART1.
 */

#ifndef __UART_H___
#define __UART_H___

#include "tm4c123gh6pm.h"

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

#endif
