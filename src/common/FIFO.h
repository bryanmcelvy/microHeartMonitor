/**
 * @addtogroup fifo
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   FIFO buffer data structure.
 */

#ifndef __FIFO_H__
#define __FIFO_H__

#include <stdint.h>

typedef struct FIFO_t FIFO_t;

/**
 * @brief               Initialize a FIFO buffer of length `N`.
 * 
 * @param fifo_ptr      Pointer to FIFO object
 * @param buffer        Array to be used as FIFO buffer
 * @param N             Length of `buffer`. Usable length is `N - 1`.
 * @return FIFO_t* 
 */
FIFO_t * FIFO_Init(FIFO_t * fifo_ptr, uint16_t buffer[], uint16_t N);

/**
 * @brief               Add a value to the end of the buffer.
 * 
 * @param fifo_ptr      Pointer to FIFO object
 * @param val           last value in the buffer
 */
void FIFO_Put(FIFO_t * fifo_ptr, uint16_t val);

/**
 * @brief               Remove the first value of the buffer.
 * 
 * @param fifo_ptr      Pointer to FIFO object
 * @return uint16_t 
 */
uint16_t FIFO_Get(FIFO_t * fifo_ptr);

/**
 * @brief               Empty the FIFO buffer's contents.
 * 
 * @param fifo_ptr      Pointer to FIFO object
 * @param output_buffer Array to output values to.
 *                      Should be the same length as the FIFO buffer.
 */
void FIFO_Flush(FIFO_t * fifo_ptr, uint16_t output_buffer[]);

#endif // __FIFO_H__

/** @} */
