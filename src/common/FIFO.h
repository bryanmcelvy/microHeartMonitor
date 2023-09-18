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
        Peeking
        Status Checks
*******************************************************************************/

/******************************************************************************
Preprocessor Directives
*******************************************************************************/

#ifndef FIFO_H
#define FIFO_H

#include "NewAssert.h"

#include <stdbool.h>
#include <stdint.h>

// Number of pre-allocated FIFO buffer can be defined at compile-time
// (i.e. "arm-none-eabi-gcc -DFIFO_POOL_SIZE=<VALUE> ...") or hard-coded
#ifndef FIFO_POOL_SIZE
#define FIFO_POOL_SIZE 5               // default val
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
FIFO_t * FIFO_Init(volatile uint32_t buffer[], const uint32_t N);

/******************************************************************************
Basic Operations
*******************************************************************************/
/** @name Basic Operations */               /// @{

/**
 * @brief               Add a value to the end of the buffer.
 *
 * @param fifo_ptr      Pointer to FIFO object
 * @param val           last value in the buffer
 */
void FIFO_Put(volatile FIFO_t * fifo_ptr, const uint32_t val);

/**
 * @brief               Remove the first value of the buffer.
 *
 * @param fifo_ptr      Pointer to FIFO object
 * @return              First sample in the FIFO.
 */
uint32_t FIFO_Get(volatile FIFO_t * fifo_ptr);

/**
 * @brief               Transfer a value from one FIFO buffer to another.
 *
 * @param src_fifo_ptr  Pointer to source FIFO buffer.
 * @param dest_fifo_ptr Pointer to destination FIFO buffer.
 */
void FIFO_TransferOne(volatile FIFO_t * src_fifo_ptr, volatile FIFO_t * dest_fifo_ptr);

///@}

/******************************************************************************
Bulk Removal
*******************************************************************************/
/** @name Bulk Removal */               /// @{

/**
 * @brief               Empty the FIFO buffer's contents into an array.
 *
 * @param fifo_ptr      Pointer to source FIFO buffer.
 * @param output_buffer Array to output values to.
 *                      Should be the same length as the FIFO buffer.
 */
void FIFO_Flush(volatile FIFO_t * fifo_ptr, uint32_t output_buffer[]);

/**
 * @brief               Reset the FIFO buffer.
 *
 * @param[in] fifo_ptr  Pointer to FIFO buffer.
 */
void FIFO_Reset(volatile FIFO_t * fifo_ptr);

/**
 * @brief               Transfer the contents of one FIFO buffer to another.
 *
 * @param src_fifo_ptr  Pointer to source FIFO buffer.
 * @param dest_fifo_ptr Pointer to destination FIFO buffer.
 */
void FIFO_TransferAll(volatile FIFO_t * src_fifo_ptr, volatile FIFO_t * dest_fifo_ptr);

///@}

/******************************************************************************
Peeking
*******************************************************************************/
/** @name Peeking */               /// @{

/**
 * @brief               See the first element in the FIFO without removing it.
 *
 * @param fifo_ptr      Pointer to FIFO object
 * @return              First sample in the FIFO.
 */
uint32_t FIFO_PeekOne(volatile FIFO_t * fifo_ptr);

/**
 * @brief               See the FIFO buffer's contents without removing them.
 *
 * @param fifo_ptr      Pointer to FIFO object
 * @param output_buffer Array to output values to.
 *                      Should be the same length as the FIFO buffer.
 */
void FIFO_PeekAll(volatile FIFO_t * fifo_ptr, uint32_t output_buffer[]);

///@}

/******************************************************************************
Status Checks
*******************************************************************************/
/** @name Status Checks */               /// @{

/**
 * @brief               Check if the FIFO buffer is full.
 *
 * @param fifo_ptr      Pointer to the FIFO buffer.
 * @retval true         The buffer is full.
 * @retval false        The buffer is not full.
 */
bool FIFO_isFull(volatile FIFO_t * fifo_ptr);

/**
 * @brief               Check if the FIFO buffer is empty.
 *
 * @param fifo_ptr      Pointer to the FIFO buffer.
 * @retval true         The buffer is empty.
 * @retval false        The buffer is not empty.
 */
bool FIFO_isEmpty(volatile FIFO_t * fifo_ptr);

/**
 * @brief               Get the current size of the FIFO buffer.
 *
 * @param fifo_ptr      Pointer to the FIFO buffer.
 */
uint32_t FIFO_getCurrSize(volatile FIFO_t * fifo_ptr);

///@}

#endif               // FIFO_H

/** @} */
