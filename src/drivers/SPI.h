/**
 * @addtogroup  spi
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Header file for serial peripheral interface (SPI) module.
 */

/******************************************************************************
TODO
        – Refactor to use more generic interface OR create separate SSI module
        as a dependency
*******************************************************************************/

#ifndef SPI_H
#define SPI_H

#include "tm4c123gh6pm.h"

#include <stdbool.h>
#include <stdint.h>

#define SPI_CLEAR_RESET() (GPIO_PORTA_DATA_R &= ~(0x80))
#define SPI_SET_RESET()   (GPIO_PORTA_DATA_R |= 0x80)

/**
 * @brief   Initialize SSI0 to act as an SPI Controller (AKA Master) in mode 0.
 *
 */
void SPI_Init(void);

/**
 * @brief                       Read data from the serial port.
 *
 * @pre                         Initialize the SPI module.
 *
 * @param[out] data             8-bit data received from the hardware's receive FIFO.
 */
uint8_t SPI_Read(void);

/**
 * @brief                       Write a command to the serial port.
 *
 * @pre                         Initialize the SPI module.
 *
 * @param[in] cmd               8-bit command to write.
 *
 * @post                        The D/C pin is cleared.
 * @post                        The data is added to the hardware's transmit FIFO.
 */
void SPI_WriteCmd(uint8_t cmd);

/**
 * @brief                       Write data to the serial port.
 *
 * @pre                         Initialize the SPI module.
 *
 * @param[in] data              8-bit data to write.
 *
 * @post                        The D/C pin is set.
 * @post                        The data is added to the hardware's transmit FIFO.
 */
void SPI_WriteData(uint8_t data);

#endif               // SPI_H

/** @} */
