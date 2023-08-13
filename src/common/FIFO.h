/**
 * @addtogroup fifo
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   FIFO buffer data structure.
 */

/******************************************************************************
SECTIONS
        Preprocessor Directives
        Type Declaration + Initialization
        Basic Operations
        Bulk Removal
        Status Checks
*******************************************************************************/

/******************************************************************************
Preprocessor Directives
*******************************************************************************/

#ifndef __FIFO_H__
#define __FIFO_H__

#include <stdint.h>
#include <stdbool.h>

// Number of pre-allocated FIFO buffer structs can be defined at compile-time
// (i.e. "arm-none-eabi-gcc -DFIFO_POOL_SIZE=<VALUE> ...")
#ifndef FIFO_POOL_SIZE
#define FIFO_POOL_SIZE 3                    // default val
#endif

/******************************************************************************
Type Declaration + Initialization
*******************************************************************************/

typedef struct FIFO_t FIFO_t;

/**
 * @brief               Initialize a FIFO buffer of length `N`.
 *
 * @param buffer        Array of size `N` to be used as FIFO buffer
 * @param N             Length of `buffer`. Usable length is `N - 1`.
 * @return              pointer to the FIFO buffer
 */
FIFO_t * FIFO_Init(uint32_t buffer[], uint32_t N);

/******************************************************************************
Basic Operations
*******************************************************************************/

/**
 * @brief               Add a value to the end of the buffer.
 *
 * @param fifo_ptr      Pointer to FIFO object
 * @param val           last value in the buffer
 */
void FIFO_Put(FIFO_t * fifo_ptr, uint32_t val);

/**
 * @brief               Remove the first value of the buffer.
 *
 * @param fifo_ptr      Pointer to FIFO object
 * @return              First sample in the FIFO.
 */
uint32_t FIFO_Get(FIFO_t * fifo_ptr);

/**
 * @brief               Transfer a value from one FIFO buffer to another.
 *
 * @param src_fifo_ptr  Pointer to source FIFO buffer.
 * @param dest_fifo_ptr Pointer to destination FIFO buffer.
 */
void FIFO_TransferOne(FIFO_t * src_fifo_ptr, FIFO_t * dest_fifo_ptr);

/******************************************************************************
Bulk Removal
*******************************************************************************/

/**
 * @brief               Empty the FIFO buffer's contents into an array.
 *
 * @param fifo_ptr      Pointer to source FIFO buffer.
 * @param output_buffer Array to output values to.
 *                      Should be the same length as the FIFO buffer.
 */
void FIFO_Flush(FIFO_t * fifo_ptr, uint32_t output_buffer[]);

/**
 * @brief               Transfer the contents of one FIFO buffer to another.
 *
 * @param src_fifo_ptr  Pointer to source FIFO buffer.
 * @param dest_fifo_ptr Pointer to destination FIFO buffer.
 */
void FIFO_TransferAll(FIFO_t * src_fifo_ptr, FIFO_t * dest_fifo_ptr);

/******************************************************************************
Status Checks
*******************************************************************************/

/**
 * @brief               See the FIFO buffer's contents without removing them.
 *
 * @param fifo_ptr      Pointer to FIFO object
 * @param output_buffer Array to output values to.
 *                      Should be the same length as the FIFO buffer.
 */
void FIFO_Peek(FIFO_t * fifo_ptr, uint32_t output_buffer[]);

/**
 * @brief               Check if the FIFO buffer is full.
 *
 * @param fifo_ptr      Pointer to the FIFO buffer.
 * @retval true         The buffer is full.
 * @retval false        The buffer is not full.
 */
bool FIFO_isFull(FIFO_t * fifo_ptr);

/**
 * @brief               Check if the FIFO buffer is empty.
 *
 * @param fifo_ptr      Pointer to the FIFO buffer.
 * @retval true         The buffer is empty.
 * @retval false        The buffer is not empty.
 */
bool FIFO_isEmpty(FIFO_t * fifo_ptr);

/**
 * @brief               Get the current size of the FIFO buffer.
 *
 * @param fifo_ptr      Pointer to the FIFO buffer.
 */
uint32_t FIFO_getCurrSize(FIFO_t * fifo_ptr);

#endif                    // __FIFO_H__

/** @} */
