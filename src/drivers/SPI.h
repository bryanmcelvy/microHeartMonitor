/**
 * @addtogroup  spi
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Driver module for using the serial peripheral interface (SPI) protocol.
 */

#ifndef __SPI_H___
#define __SPI_H___

/******************************************************************************
SECTIONS
        Preprocessor Directives
        Initialization
        Basic Operations (Polling-based)
        Basic Operations (Interrupt-based)
*******************************************************************************/

/******************************************************************************
Preprocessor Directives
*******************************************************************************/

#include "tm4c123gh6pm.h"

#include "FIFO.h"

#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
Initialization
*******************************************************************************/

/**
 * @brief   Initialize SSI0 to act as an SPI Controller (AKA Master) in mode 0.
 *
 */
void SPI_Init(void);

/******************************************************************************
Basic Operations (Polling-based)
*******************************************************************************/

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

/*
 * @brief                   Write a sequence of data to the peripheral,
 *                          with or without a preceding command.
 *
 * @param cmd               8-bit command (using `cmd = 0` omits the command)
 * @param param_sequence    sequence of parameters to send after `cmd`
 * @param num_params        number of parameters to send; should be <= size of `param_sequence`
 */
// void SPI_WriteSequence(uint8_t cmd, uint8_t * param_sequence, uint8_t num_params);

/******************************************************************************
Basic Operations (Interrupt-based)
*******************************************************************************/

/**
 * @brief                   Add an 8-bit command to the SPI queue.
 *                          If no data or other command is written, should
 *                          directly precede a call to `SPI_IRQ_StartWriting()`.
 *
 * @param cmd               command for peripheral
 */
void SPI_IRQ_WriteCmd(uint8_t cmd);

/**
 * @brief                   Add 8-bit data to the SPI queue.
 *                          Should directly precede either another call to the
 *                          same function or a call to `SPI_IRQ_StartWriting()`.
 *
 * @param data              input data for peripheral
 */
void SPI_IRQ_WriteData(uint8_t data);

/**
 * @brief                   Start writing data to the Tx FIFO.
 *                          Should be used after 1+ calls to `SPI_IRQ_WriteCmd()`
 *                          and/or `SPI_IRQ_WriteData()`. If unused, writing will
 *                          start when the SPI queue is full.
 */
void SPI_IRQ_StartWriting(void);

#endif

/** @} */
