/**
 * @file
 * @author  Bryan McElvy
 * @brief   Driver module for using the serial peripheral interface (SPI) protocol.
 */

#ifndef __SPI_H___
#define __SPI_H___

#include "tm4c123gh6pm.h"

/**
 * @brief   Initialize SSI0 to act as an SPI Controller (AKA Master) in mode 0.
 * 
 */
void SPI_Init(void);

#endif
