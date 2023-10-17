/**
 * @addtogroup fifo
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for FIFO buffer module.
 */

#include "FIFO.h"

/******************************************************************************
SECTIONS
        Initialization
        Basic Operations (Int)
        Basic Operations (Float)
        Peeking
        Status Checks
*******************************************************************************/

#include "NewAssert.h"

#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
Initialization
*******************************************************************************/

typedef struct FifoStruct_t {
    volatile uint32_t * buffer;               ///< (pointer to) array to use as FIFO buffer
    volatile uint32_t N;                      ///< length of `buffer`
    volatile uint32_t frontIdx;               ///< idx of front of FIFO
    volatile uint32_t backIdx;                ///< idx of back of FIFO
} FifoStruct_t;

static FifoStruct_t fifoPool[FIFO_POOL_SIZE] = { 0 };               ///< pre-allocated pool
static uint8_t numFreeFifos = FIFO_POOL_SIZE;

Fifo_t FIFO_Init(volatile uint32_t buffer[], const uint32_t N) {
    /// TODO: Add details
    volatile Fifo_t fifo = 0;

    Assert(numFreeFifos > 0);
    numFreeFifos -= 1;

    fifo = &(fifoPool[numFreeFifos]);
    fifo->buffer = buffer;
    fifo->N = N;
    fifo->frontIdx = 0;
    fifo->backIdx = 0;

    return fifo;
}

void FIFO_Reset(volatile Fifo_t fifo) {
    fifo->backIdx = fifo->frontIdx;
    return;
}

/******************************************************************************
Basic Operations (Int)
*******************************************************************************/

void FIFO_Put(volatile Fifo_t fifo, const uint32_t val) {
    // NOTE: not using FIFO_isFull() here to reduce call stack usage
    bool isFifoFull = (((fifo->backIdx + 1) % fifo->N) == fifo->frontIdx) ? true : false;

    if(isFifoFull == false) {
        fifo->buffer[fifo->backIdx] = val;
        fifo->backIdx = (fifo->backIdx + 1) % fifo->N;
    }

    return;
}

uint32_t FIFO_Get(volatile Fifo_t fifo) {
    uint32_t val;

    // NOTE: not using FIFO_isEmpty() here to reduce call stack usage
    bool isFifoEmpty = (fifo->frontIdx == fifo->backIdx) ? true : false;

    if(isFifoEmpty) {
        val = 0;
    }
    else {
        val = fifo->buffer[fifo->frontIdx];
        fifo->frontIdx = (fifo->frontIdx + 1) % fifo->N;
    }

    return val;
}

void FIFO_Flush(volatile Fifo_t fifo, uint32_t outputBuffer[]) {
    uint32_t idx = 0;

    while(FIFO_isEmpty(fifo) == false) {
        outputBuffer[idx++] = fifo->buffer[fifo->frontIdx];
        fifo->frontIdx = (fifo->frontIdx + 1) % fifo->N;               // wrap around to end
    }
}

/******************************************************************************
Basic Operations (Float)
*******************************************************************************/

void FIFO_PutFloat(volatile Fifo_t fifo, const float val) {
    /// @remark To properly use floating-point values, type-punning is necessary.
    FIFO_Put(fifo, *((uint32_t *) &val));
    return;
}

float FIFO_GetFloat(volatile Fifo_t fifo) {
    /// @remark To properly use floating-point values, type-punning is necessary.
    float val;
    *((uint32_t *) &val) = FIFO_Get(fifo);
    return val;
}

void FIFO_FlushFloat(volatile Fifo_t fifo, float outputBuffer[]) {
    FIFO_Flush(fifo, (uint32_t *) outputBuffer);
    return;
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
        ret_val = fifo->buffer[fifo->frontIdx];
    }

    return ret_val;
}

void FIFO_PeekAll(volatile Fifo_t fifo, uint32_t outputBuffer[]) {
    uint32_t frontIdx = fifo->frontIdx;
    uint32_t idx = 0;

    while(frontIdx != fifo->backIdx) {
        outputBuffer[idx] = fifo->buffer[frontIdx];
        idx += 1;

        frontIdx = (frontIdx + 1) % fifo->N;               // wrap around to end
    }

    return;
}

/******************************************************************************
Status Checks
*******************************************************************************/

bool FIFO_isFull(volatile Fifo_t fifo) {
    return (bool) (((fifo->backIdx + 1) % fifo->N) == fifo->frontIdx);
}

bool FIFO_isEmpty(volatile Fifo_t fifo) {
    return (bool) (fifo->frontIdx == fifo->backIdx);
}

uint32_t FIFO_getCurrSize(volatile Fifo_t fifo) {
    uint32_t size;

    if(FIFO_isEmpty(fifo)) {
        size = 0;
    }
    else if(FIFO_isFull(fifo)) {
        size = fifo->N - 1;
    }
    else if(fifo->frontIdx < fifo->backIdx) {
        size = fifo->backIdx - fifo->frontIdx;
    }
    else {
        size = fifo->N - (fifo->frontIdx - fifo->backIdx);
    }

    return size;
}

/** @} */
