/**
 * @addtogroup fifo
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Header file for FIFO buffer implementation.
 */

#ifndef FIFO_H
#define FIFO_H

/******************************************************************************
SECTIONS
        Preprocessor Directives
        Initialization
        Basic Operations (Int)
        Basic Operations (Float)
        Peeking
        Status Checks
*******************************************************************************/

/******************************************************************************
Preprocessor Directives
*******************************************************************************/

#include <stdbool.h>
#include <stdint.h>

// Number of pre-allocated FIFO buffers can be defined at compile-time
// (e.g. "arm-none-eabi-gcc -DFIFO_POOL_SIZE=<VALUE> ...") or hard-coded
#ifndef FIFO_POOL_SIZE
#define FIFO_POOL_SIZE 5               // default val
#endif

/******************************************************************************
Initialization
*******************************************************************************/

typedef struct FifoStruct_t * Fifo_t;

/**
 * @brief               Initialize a FIFO buffer of length `N`.
 *
 * @param[in] buffer    Array of size `N` to be used as FIFO buffer
 * @param[in] N         Length of `buffer`. Usable length is `N - 1`.
 * @param[out] fifo     pointer to the FIFO buffer
 *
 * @post                The number of available FIFO buffers is reduced by 1.
 */
Fifo_t FIFO_Init(volatile uint32_t buffer[], const uint32_t N);

/**
 * @brief               Reset the FIFO buffer.
 *
 * @param[in] fifo      Pointer to FIFO buffer.
 *
 * @post                The FIFO is now considered empty.
 *                      The underlying buffer's contents are not affected.
 */
void FIFO_Reset(volatile Fifo_t fifo);

/******************************************************************************
Basic Operations (Int)
*******************************************************************************/
/** @name Basic Operations */               /// @{

/**
 * @brief               Add a value to the end of the buffer.
 *
 * @param[in] fifo      Pointer to FIFO object
 * @param[in] val       Value to add to the buffer.
 *
 * @post                If the FIFO is not full, `val` is placed in the buffer.
 *                      If the FIFO is full, nothing happens.
 *
 * @see                 FIFO_PutFloat()
 */
void FIFO_Put(volatile Fifo_t fifo, const uint32_t val);

/**
 * @brief               Remove the first value of the buffer.
 *
 * @param[in] fifo      Pointer to FIFO object
 * @param[out] val      First sample in the FIFO.
 *
 * @post                If the FIFO is not empty, the next value is returned.
 *                      If the FIFO is empty, `0` is returned.
 *
 * @see                 FIFO_GetFloat()
 */
uint32_t FIFO_Get(volatile Fifo_t fifo);

/**
 * @brief                       Empty the FIFO buffer's contents into an array.
 *
 * @param[in] fifo              Pointer to source FIFO buffer.
 * @param[in] outputBuffer      Array to output values to.
 *                              Should be the same length as the FIFO buffer.
 *
 * @post                        The FIFO buffer's contents are transferred to the output buffer.
 *
 * @see                         FIFO_FlushFloat()
 */
void FIFO_Flush(volatile Fifo_t fifo, uint32_t outputBuffer[]);

/******************************************************************************
Basic Operations (Float)
*******************************************************************************/

/**
 * @brief               Add a floating-point value to the end of the buffer.
 *
 * @param[in] fifo      Pointer to FIFO object
 * @param[in] val       Value to add to the buffer.
 *
 * @post                If the FIFO is not full, `val` is placed in the buffer.
 *                      If the FIFO is full, nothing happens.
 *
 * @note                This was added to avoid needing to type-pun floating-point values.
 * @code
 *      // type-punning example
 *      float num = 4.252603;
 *      FIFO_Put(fifo, *((uint32_t *) &num));
 *      FIFO_PutFloat(fifo, num); // same thing, but cleaner
 * @endcode
 *
 * @see                 FIFO_Put()
 */
void FIFO_PutFloat(volatile Fifo_t fifo, const float val);

/**
 * @brief               Remove the first value of the buffer, and cast it to `float`.
 *
 * @param[in] fifo      Pointer to FIFO object
 * @param[out] val      First sample in the FIFO.
 *
 * @post                If the FIFO is not empty, the next value is returned.
 *                      If the FIFO is empty, `0` is returned.
 *
 * @note                This was added to avoid needing to type-pun floating-point values.
 * @code
 *      // type-punning example
 *      float num;
 *      *((uint32_t *) &num) = FIFO_Get(fifo);
 *      num = FIFO_GetFloat(fifo);
 * @endcode
 *
 * @see                 FIFO_Get()
 */
float FIFO_GetFloat(volatile Fifo_t fifo);

/**
 * @brief                       Empty the FIFO buffer into an array of floating-point values.
 *
 * @param[in] fifo              Pointer to source FIFO buffer.
 * @param[in] outputBuffer      Array to output values to.
 *                              Should be the same length as the FIFO buffer.
 *
 * @post                        The FIFO buffer's contents are transferred to the output buffer.
 *
 * @note                        This was added to avoid needing to type-pun floating-point values.
 * @code
 *      // type-punning example
 *      FIFO_Flush(fifo, (uint32_t *) outputBuffer);
 *      FIFO_FlushFloat(fifo, outputBuffer); // same thing, but cleaner
 * @endcode
 *
 * @see                         FIFO_Flush()
 */
void FIFO_FlushFloat(volatile Fifo_t fifo, float outputBuffer[]);

/** @} */               // Basic Operations

/******************************************************************************
Peeking
*******************************************************************************/
/** @name Peeking */               /// @{

/**
 * @brief               See the first element in the FIFO without removing it.
 *
 * @param[in] fifo      Pointer to FIFO object
 * @param[out] val      First sample in the FIFO.
 */
uint32_t FIFO_PeekOne(volatile Fifo_t fifo);

/**
 * @brief                       See the FIFO buffer's contents without removing them.
 *
 * @param[in] fifo              Pointer to source FIFO buffer.
 * @param[in] outputBuffer      Array to output values to.
 *                              Should be the same length as the FIFO buffer.
 *
 * @post                        The FIFO buffer's contents are copied to the output buffer.
 */
void FIFO_PeekAll(volatile Fifo_t fifo, uint32_t outputBuffer[]);

/** @} */               // Peeking

/******************************************************************************
Status Checks
*******************************************************************************/
/** @name Status Checks */               /// @{

/**
 * @brief               Check if the FIFO buffer is full.
 *
 * @param[in] fifo      Pointer to the FIFO buffer.
 * @param[out] true     The FIFO buffer is full.
 * @param[out] false    The FIFO buffer is not full.
 */
bool FIFO_isFull(volatile Fifo_t fifo);

/**
 * @brief               Check if the FIFO buffer is empty.
 *
 * @param[in] fifo      Pointer to the FIFO buffer.
 * @param[out] true     The FIFO buffer is empty.
 * @param[out] false    The FIFO buffer is not empty.
 */
bool FIFO_isEmpty(volatile Fifo_t fifo);

/**
 * @brief               Get the current size of the FIFO buffer.
 *
 * @param[in] fifo      Pointer to the FIFO buffer.
 * @param[out] size     Current number of values in the FIFO buffer.
 */
uint32_t FIFO_getCurrSize(volatile Fifo_t fifo);

/** @} */               // Status Checks

#endif                  // FIFO_H

/** @} */
