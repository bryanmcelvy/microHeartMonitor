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
 * @brief           Initialize a FIFO buffer of length `N`.
 * 
 * @param fifo_ptr  pointer to FIFO object
 * @param buffer    array to be used as FIFO buffer
 * @param N         = (length of buffer) - 1; Should be <= length of buffer
 * @return FIFO_t* 
 */
FIFO_t * FIFO_Init(FIFO_t * fifo_ptr, uint16_t buffer[], uint16_t N);

/**
 * @brief           Remove the first value of the buffer.
 * 
 * @param fifo_ptr  pointer to FIFO object
 * @return          uint16_t 
 */
uint16_t FIFO_Get(FIFO_t * fifo_ptr);

/**
 * @brief           Add a value to the end of the buffer.
 * 
 * @param fifo_ptr  pointer to FIFO object
 * @param val       last value in the buffer
 */
void FIFO_Put(FIFO_t * fifo_ptr, uint16_t val);

#endif // __FIFO_H__

/** @} */
