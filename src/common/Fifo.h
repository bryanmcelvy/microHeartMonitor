/**
 * @addtogroup fifo
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   FIFO buffer data structure.
 */

#ifndef FIFO_H
#define FIFO_H

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

#include <stdbool.h>
#include <stdint.h>

// Number of pre-allocated FIFO buffers can be defined at compile-time
// (i.e. "arm-none-eabi-gcc -DFIFO_POOL_SIZE=<VALUE> ...") or hard-coded
#ifndef FIFO_POOL_SIZE
#define FIFO_POOL_SIZE 5               // default val
#endif

/******************************************************************************
Type Declaration + Initialization
*******************************************************************************/

typedef struct FifoStruct_t * Fifo_t;

/**
 * @brief               Initialize a FIFO buffer of length `N`.
 *
 * @param buffer        Array of size `N` to be used as FIFO buffer
 * @param N             Length of `buffer`. Usable length is `N - 1`.
 * @return              pointer to the FIFO buffer
 */
Fifo_t FIFO_Init(volatile uint32_t buffer[], const uint32_t N);

/******************************************************************************
Basic Operations
*******************************************************************************/
/** @name Basic Operations */               /// @{

/**
 * @brief               Add a value to the end of the buffer.
 *
 * @param fifo          Pointer to FIFO object
 * @param val           last value in the buffer
 */
void FIFO_Put(volatile Fifo_t fifo, const uint32_t val);

/**
 * @brief               Remove the first value of the buffer.
 *
 * @param fifo          Pointer to FIFO object
 * @return              First sample in the FIFO.
 */
uint32_t FIFO_Get(volatile Fifo_t fifo);

/**
 * @brief               Transfer a value from one FIFO buffer to another.
 *
 * @param srcFifo       Pointer to source FIFO buffer.
 * @param destFifo      Pointer to destination FIFO buffer.
 */
void FIFO_TransferOne(volatile Fifo_t srcFifo, volatile Fifo_t destFifo);

///@}

/******************************************************************************
Bulk Removal
*******************************************************************************/
/** @name Bulk Removal */               /// @{

/**
 * @brief               Empty the FIFO buffer's contents into an array.
 *
 * @param fifo          Pointer to source FIFO buffer.
 * @param outputBuffer  Array to output values to.
 *                      Should be the same length as the FIFO buffer.
 */
void FIFO_Flush(volatile Fifo_t fifo, uint32_t outputBuffer[]);

/**
 * @brief               Reset the FIFO buffer.
 *
 * @param[in] fifo  Pointer to FIFO buffer.
 */
void FIFO_Reset(volatile Fifo_t fifo);

/**
 * @brief               Transfer the contents of one FIFO buffer to another.
 *
 * @param srcFifo       Pointer to source FIFO buffer.
 * @param destFifo      Pointer to destination FIFO buffer.
 */
void FIFO_TransferAll(volatile Fifo_t srcFifo, volatile Fifo_t destFifo);

///@}

/******************************************************************************
Peeking
*******************************************************************************/
/** @name Peeking */               /// @{

/**
 * @brief               See the first element in the FIFO without removing it.
 *
 * @param fifo      Pointer to FIFO object
 * @return              First sample in the FIFO.
 */
uint32_t FIFO_PeekOne(volatile Fifo_t fifo);

/**
 * @brief               See the FIFO buffer's contents without removing them.
 *
 * @param fifo          Pointer to FIFO object
 * @param outputBuffer  Array to output values to.
 *                      Should be the same length as the FIFO buffer.
 */
void FIFO_PeekAll(volatile Fifo_t fifo, uint32_t outputBuffer[]);

///@}

/******************************************************************************
Status Checks
*******************************************************************************/
/** @name Status Checks */               /// @{

/**
 * @brief               Check if the FIFO buffer is full.
 *
 * @param fifo      Pointer to the FIFO buffer.
 * @retval true         The buffer is full.
 * @retval false        The buffer is not full.
 */
bool FIFO_isFull(volatile Fifo_t fifo);

/**
 * @brief               Check if the FIFO buffer is empty.
 *
 * @param fifo      Pointer to the FIFO buffer.
 * @retval true         The buffer is empty.
 * @retval false        The buffer is not empty.
 */
bool FIFO_isEmpty(volatile Fifo_t fifo);

/**
 * @brief               Get the current size of the FIFO buffer.
 *
 * @param fifo      Pointer to the FIFO buffer.
 */
uint32_t FIFO_getCurrSize(volatile Fifo_t fifo);

///@}

#endif               // FIFO_H

/** @} */
