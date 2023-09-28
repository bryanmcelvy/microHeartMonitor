/**
 * @addtogroup  spi
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Driver module for using the serial peripheral interface (SPI) protocol.
 */

#ifndef SPI_H
#define SPI_H

/******************************************************************************
TODO
        – Refactor to use more generic interface OR create separate SSI module
        as a dependency
*******************************************************************************/

#include "GPIO.h"

#include "FIFO.h"

#include "tm4c123gh6pm.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief   Initialize SSI0 to act as an SPI Controller (AKA Master) in mode 0.
 *
 */
void SPI_Init(void);

/**
 * @brief                   Read data from the peripheral.
 *
 * @return                  uint8_t
 */
uint8_t SPI_Read(void);

/**
 * @brief                   Write an 8-bit command to the peripheral.
 *
 * @param cmd               command for peripheral
 */
void SPI_WriteCmd(uint8_t cmd);

/**
 * @brief                   Write 8-bit data to the peripheral.
 *
 * @param data              input data for peripheral
 */
void SPI_WriteData(uint8_t data);

#endif               // SPI_H

/** @} */
