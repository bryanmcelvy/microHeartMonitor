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
        Initialization (Static Allocation)
        Initialization (Dynamic Allocation)
        Basic Operations (Int)
        Peeking
        Status Checks
*******************************************************************************/

/******************************************************************************
Preprocessor Directives
*******************************************************************************/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Number of pre-allocated FIFO buffers can be defined at compile-time
// (e.g. "arm-none-eabi-gcc -DFifo_POOL_SIZE=<VALUE> ...") or hard-coded
#ifndef FIFO_POOL_SIZE
#define FIFO_POOL_SIZE 5               // default val
#endif

/******************************************************************************
Initialization (Static Allocation)
*******************************************************************************/

typedef struct FifoStruct_t * Fifo_t;

Fifo_t Fifo_Init(volatile uint32_t buffer[], const uint32_t N, size_t dataSize);

void Fifo_Reset(volatile Fifo_t fifo);

/******************************************************************************
Initialization (Dynamic)
*******************************************************************************/

Fifo_t Fifo_DynCreate(const uint32_t N, const size_t dataSize);

void Fifo_DynDestroy(Fifo_t fifo);

/******************************************************************************
Basic Operations
*******************************************************************************/
/** @name Basic Operations */               /// @{

typedef enum {
    FIFO_FAILURE = false,
    FIFO_SUCCESS = true
} FifoStatus_e;

FifoStatus_e Fifo_Put(volatile Fifo_t fifo, void * inputValPtr);

FifoStatus_e Fifo_Get(volatile Fifo_t fifo, void * outputValPtr);

FifoStatus_e Fifo_Flush(volatile Fifo_t fifo, void * outputBuffer);

/******************************************************************************
Peeking
*******************************************************************************/
/** @name Peeking */               /// @{

FifoStatus_e Fifo_PeekOne(volatile Fifo_t fifo, void * outputValPtr);

FifoStatus_e Fifo_PeekAll(volatile Fifo_t fifo, void * outputBuffer);

/** @} */                          // Peeking

/******************************************************************************
Status Checks
*******************************************************************************/
/** @name Status Checks */               /// @{

bool Fifo_isFull(volatile Fifo_t fifo);

bool Fifo_isEmpty(volatile Fifo_t fifo);

uint32_t Fifo_getCurrSize(volatile Fifo_t fifo);

/** @} */                                // Status Checks

#endif                                   // Fifo_H

/** @} */
