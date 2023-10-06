/**
 * @addtogroup fifo
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for FIFO buffer module.
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

#include "FIFO.h"

#include "NewAssert.h"

#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
Type Declaration + Initialization
*******************************************************************************/

typedef struct FifoStruct_t {
    volatile uint32_t * buffer;                ///< (pointer to) array to use as FIFO buffer
    volatile uint32_t N;                       ///< length of `buffer`
    volatile uint32_t front_idx;               ///< idx of front of FIFO
    volatile uint32_t back_idx;                ///< idx of back of FIFO
} FifoStruct_t;

static FifoStruct_t buffer_pool[FIFO_POOL_SIZE] = { 0 };               ///< pre-allocated pool
static uint8_t free_buffers = FIFO_POOL_SIZE;                          ///< no. of remaining buffers

Fifo_t FIFO_Init(volatile uint32_t buffer[], const uint32_t N) {
    /// TODO: Add details
    volatile Fifo_t fifo = 0;

    Assert(free_buffers > 0);
    fifo = &(buffer_pool[--free_buffers]);
    fifo->buffer = buffer;
    fifo->N = N;
    fifo->front_idx = 0;
    fifo->back_idx = 0;

    return fifo;
}

/******************************************************************************
Basic Operations
*******************************************************************************/

void FIFO_Put(volatile Fifo_t fifo, const uint32_t val) {
    // NOTE: not using FIFO_isFull() here to reduce call stack usage
    if(((fifo->back_idx + 1) % fifo->N) != fifo->front_idx) {
        fifo->buffer[fifo->back_idx] = val;
        fifo->back_idx =
            (fifo->back_idx + 1) % fifo->N;               // modulo causes wrap around to 0
    }
}

uint32_t FIFO_Get(volatile Fifo_t fifo) {
    volatile uint32_t ret_val;

    // NOTE: not using FIFO_isEmpty() here to reduce call stack usage
    if(fifo->front_idx == fifo->back_idx) {
        ret_val = 0;
    }
    else {
        ret_val = fifo->buffer[fifo->front_idx];
        fifo->front_idx =
            (fifo->front_idx + 1) % fifo->N;               // modulo causes wrap around to 0
    }

    return ret_val;
}

void FIFO_TransferOne(volatile Fifo_t srcFifo, volatile Fifo_t destFifo) {
    if((FIFO_isEmpty(srcFifo) == false) && (FIFO_isFull(destFifo) == false)) {
        FIFO_Put(destFifo, FIFO_Get(srcFifo));
    }
}

/******************************************************************************
Bulk Removal
*******************************************************************************/

void FIFO_Flush(volatile Fifo_t fifo, uint32_t outputBuffer[]) {
    uint32_t idx = 0;

    while(FIFO_isEmpty(fifo) == false) {
        outputBuffer[idx++] = fifo->buffer[fifo->front_idx];
        fifo->front_idx = (fifo->front_idx + 1) % fifo->N;               // wrap around to end
    }
}

void FIFO_Reset(volatile Fifo_t fifo) {
    fifo->back_idx = fifo->front_idx;
    return;
}

void FIFO_TransferAll(volatile Fifo_t srcFifo, volatile Fifo_t destFifo) {
    while((FIFO_isEmpty(srcFifo) == false) && (FIFO_isFull(destFifo) == false)) {
        FIFO_Put(destFifo, FIFO_Get(srcFifo));
    }
}

/******************************************************************************
Peeking
*******************************************************************************/

uint32_t FIFO_PeekOne(volatile Fifo_t fifo) {
    uint32_t ret_val;

    if(FIFO_isEmpty(fifo)) {
        ret_val = 0;
    }
    else {
        ret_val = fifo->buffer[fifo->front_idx];
    }

    return ret_val;
}

void FIFO_PeekAll(volatile Fifo_t fifo, uint32_t outputBuffer[]) {
    uint32_t temp_front_idx = fifo->front_idx;
    uint32_t idx = 0;

    while(temp_front_idx != fifo->back_idx) {
        outputBuffer[idx++] = fifo->buffer[temp_front_idx];
        temp_front_idx = (temp_front_idx + 1) % fifo->N;               // wrap around to end
    }
}

/******************************************************************************
Status Checks
*******************************************************************************/

bool FIFO_isFull(volatile Fifo_t fifo) {
    return (bool) (((fifo->back_idx + 1) % fifo->N) == fifo->front_idx);
}

bool FIFO_isEmpty(volatile Fifo_t fifo) {
    return (bool) (fifo->front_idx == fifo->back_idx);
}

uint32_t FIFO_getCurrSize(volatile Fifo_t fifo) {
    uint32_t size;

    if(FIFO_isEmpty(fifo)) {
        size = 0;
    }
    else if(FIFO_isFull(fifo)) {
        size = fifo->N - 1;
    }
    else if(fifo->front_idx < fifo->back_idx) {
        size = fifo->back_idx - fifo->front_idx;
    }
    else {
        size = fifo->N - (fifo->front_idx - fifo->back_idx);
    }

    return size;
}

/** @} */
