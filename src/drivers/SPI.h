/**
 * @addtogroup  spi
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Header file for serial peripheral interface (SPI) module.
 */

#ifndef SPI_H
#define SPI_H

#include "GPIO.h"

#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
Initialization
*******************************************************************************/

typedef struct SpiStruct_t * Spi_t;

typedef enum {
    SSI0,
    SSI1,
    SSI2,
    SSI3
} SsiNum_t;

/**
 * @brief                       Initialize an SSI as an SPI controller.
 *
 * @param[in] gpioPort          GPIO port to use.
 * @param[in] dcPin             GPIO pin to use.
 * @param[in] ssiNum            SSI to use.
 * @param[out] Spi_t            (Pointer to) initialized SPI peripheral.
 */
Spi_t SPI_Init(GpioPort_t gpioPort, GpioPin_t dcPin, SsiNum_t ssiNum);

/**
 * @brief                       Check if a given SPI is initialized.
 *
 * @param[in] spi               SPI to check.
 * @param[out] true             The SPI is initialized.
 * @param[out] false            The SPI is not initialized.
 */
bool SPI_isInit(Spi_t spi);

/******************************************************************************
Configuration
*******************************************************************************/

typedef enum {
    SPI_RISING_EDGE,
    SPI_FALLING_EDGE
} SpiClockPhase_t;

typedef enum {
    SPI_STEADY_STATE_LOW,
    SPI_STEADY_STATE_HIGH
} SpiClockPolarity_t;

/**
 * @brief                       Configure an SPI's clock settings.
 *
 * @pre                         Initialize the SPI.
 * @pre                         Disable the SPI.
 *
 * @param[in] spi               SPI to configure.
 * @param[in] clockPhase
 * @param[in] clockPolarity
 */
void SPI_configClock(Spi_t spi, SpiClockPhase_t clockPhase, SpiClockPolarity_t clockPolarity);

/**
 * @brief
 *
 * @pre                         Initialize the SPI.
 * @pre                         Disable the SPI.
 *
 * @param[in] spi
 * @param[in] dataSize
 */
void SPI_setDataSize(Spi_t spi, uint8_t dataSize);

/**
 * @brief                       Enable an SPI.
 *
 * @pre                         Initialize the SPI.
 * @param[in] spi               SPI to enable.
 * @post                        The SPI is enable.
 *
 * @see                         SPI_Disable()
 */
void SPI_Enable(Spi_t spi);

/**
 * @brief                       Disable an SPI.
 *
 * @pre                         Initialize the SPI.
 * @param[in] spi               SPI to disable.
 * @post                        The SPI is disabled.
 *
 * @see                         SPI_Enable()
 */
void SPI_Disable(Spi_t spi);

/******************************************************************************
Operations
*******************************************************************************/

/**
 * @brief                       Read data from the serial port.
 *
 * @pre                         Initialize the SPI.
 * @pre                         Enable the SPI.
 *
 * @param[in] spi               SPI to read from.
 * @param[out] data             8-bit data received from the hardware's receive FIFO.
 */
uint16_t SPI_Read(Spi_t spi);

/**
 * @brief                       Write a command to the serial port.
 *
 * @pre                         Initialize the SPI.
 * @pre                         Enable the SPI.
 *
 * @param[in] spi               SPI to write to.
 * @param[in] cmd               Command to write.
 *
 * @post                        The D/C pin is cleared.
 * @post                        The command is added to the hardware's transmit FIFO.
 */
void SPI_WriteCmd(Spi_t spi, uint16_t cmd);

/**
 * @brief                       Write data to the serial port.
 *
 * @pre                         Initialize the SPI.
 * @pre                         Enable the SPI.
 *
 * @param[in] spi               SPI to write to.
 * @param[in] data              Data to write.
 *
 * @post                        The D/C pin is set.
 * @post                        The data is added to the hardware's transmit FIFO.
 */
void SPI_WriteData(Spi_t spi, uint16_t data);

#endif                  // SPI_H

/** @} */               // spi
