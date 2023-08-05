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

typedef struct FIFO FIFO_t;

/**
 * @brief           Initialize a FIFO buffer of length `N`.
 * 
 * @param N         number of elements in FIFO buffer.
 * @return          FIFO_t 
 */
FIFO_t FIFO_Init(uint16_t N);

/**
 * @brief           Remove the first value of the buffer.
 * 
 * @param fifo_ptr  pointer to the buffer
 * @return          uint16_t 
 */
uint16_t FIFO_Get(FIFO_t * fifo_ptr);

/**
 * @brief           Add a value to the end of the buffer.
 * 
 * @param fifo_ptr  pointer to the buffer
 * @param val       last value in the buffer
 */
void FIFO_Add(FIFO_t * fifo_ptr, uint16_t val);

#endif // __FIFO_H__

/** @} */
