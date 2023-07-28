/**
 * @file
 * @author  Bryan McElvy
 * @brief   Driver module for using the serial peripheral interface (SPI) protocol.
 * 
 * @addtogroup  spi
 * @{
 */

#ifndef __SPI_H___
#define __SPI_H___

#include "tm4c123gh6pm.h"

/**
 * @brief   Initialize SSI0 to act as an SPI Controller (AKA Master) in mode 0.
 * 
 */
void SPI_Init(void);

/**
 * @brief   Read data from peripheral.
 * 
 * @return  uint8_t
 */
uint8_t SPI_Read(void);

/**
 * @brief   Write an 8-bit command to the peripheral.
 * 
 * @param   cmd command for peripheral
 */
void SPI_WriteCmd(uint8_t cmd);

/**
 * @brief   Write 8-bit data to the peripheral.
 * 
 * @param   data input data for peripheral
 */
void SPI_WriteData(uint8_t data);

/**
 * @brief                   Write a sequence of data to the peripheral,
 *                          with or without a preceding command.
 * 
 * @param cmd               8-bit command (using `cmd = 0` omits the command)
 * @param param_sequence    sequence of parameters to send after `cmd`
 * @param num_params        number of parameters to send; should be <= size of `param_sequence`
 */
void SPI_WriteSequence(uint8_t cmd, uint8_t param_sequence[], uint8_t num_params);

#endif

/** @} */
