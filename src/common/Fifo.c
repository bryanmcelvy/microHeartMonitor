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
#include <string.h>

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

Fifo_t Fifo_Init(volatile uint32_t buffer[], const uint32_t N) {
    Assert(numFreeFifos > 0);

    numFreeFifos -= 1;
    volatile Fifo_t fifo = &(fifoPool[numFreeFifos]);

    fifo->buffer = buffer;
    fifo->N = N;
    fifo->frontIdx = 0;
    fifo->backIdx = 0;

    return fifo;
}

void Fifo_Reset(volatile Fifo_t fifo) {
    fifo->backIdx = fifo->frontIdx;
    return;
}

/******************************************************************************
Basic Operations (Int)
*******************************************************************************/

void Fifo_Put(volatile Fifo_t fifo, const uint32_t val) {
    // NOTE: not using FIFO_isFull() here to reduce call stack usage
    if(((fifo->backIdx + 1) % fifo->N) != fifo->frontIdx) {
        memcpy(&fifo->buffer[fifo->backIdx], &val, sizeof(fifo->buffer[0]));
        fifo->backIdx = (fifo->backIdx + 1) % fifo->N;
    }

    return;
}

uint32_t Fifo_Get(volatile Fifo_t fifo) {
    uint32_t val;

    // NOTE: not using FIFO_isEmpty() here to reduce call stack usage
    if(fifo->frontIdx == fifo->backIdx) {
        val = 0;
    }
    else {
        memcpy(&val, &fifo->buffer[fifo->frontIdx], sizeof(fifo->buffer[0]));
        fifo->frontIdx = (fifo->frontIdx + 1) % fifo->N;
    }

    return val;
}

void Fifo_Flush(volatile Fifo_t fifo, uint32_t outputBuffer[]) {
    uint32_t idx = 0;

    // NOTE: not using FIFO_isEmpty() here to reduce call stack usage
    while(fifo->frontIdx != fifo->backIdx) {
        memcpy(&outputBuffer[idx], &fifo->buffer[fifo->frontIdx], sizeof(fifo->buffer[0]));
        idx += 1;
        fifo->frontIdx = (fifo->frontIdx + 1) % fifo->N;
    }

    return;
}

/******************************************************************************
Basic Operations (Float)
*******************************************************************************/

void Fifo_PutFloat(volatile Fifo_t fifo, const float val) {
    /// @remark To properly use floating-point values, type-punning is necessary.
    Fifo_Put(fifo, *((uint32_t *) &val));
    return;
}

float Fifo_GetFloat(volatile Fifo_t fifo) {
    /// @remark To properly use floating-point values, type-punning is necessary.
    float val;
    *((uint32_t *) &val) = Fifo_Get(fifo);
    return val;
}

void Fifo_FlushFloat(volatile Fifo_t fifo, float outputBuffer[]) {
    Fifo_Flush(fifo, (uint32_t *) outputBuffer);
    return;
}

/******************************************************************************
Peeking
*******************************************************************************/

uint32_t Fifo_PeekOne(volatile Fifo_t fifo) {
    uint32_t ret_val;

    if(fifo->frontIdx == fifo->backIdx) {
        ret_val = 0;
    }
    else {
        memcpy(&ret_val, &fifo->buffer[fifo->frontIdx], sizeof(fifo->buffer[0]));
    }

    return ret_val;
}

void Fifo_PeekAll(volatile Fifo_t fifo, uint32_t outputBuffer[]) {
    uint32_t frontIdx = fifo->frontIdx;
    uint32_t idx = 0;

    while(frontIdx != fifo->backIdx) {
        memcpy(&outputBuffer[idx], &fifo->buffer[frontIdx], sizeof(fifo->buffer[0]));
        idx += 1;
        frontIdx = (frontIdx + 1) % fifo->N;               // wrap around to end
    }

    return;
}

/******************************************************************************
Status Checks
*******************************************************************************/

bool Fifo_isFull(volatile Fifo_t fifo) {
    return (bool) (((fifo->backIdx + 1) % fifo->N) == fifo->frontIdx);
}

bool Fifo_isEmpty(volatile Fifo_t fifo) {
    return (bool) (fifo->frontIdx == fifo->backIdx);
}

uint32_t Fifo_getCurrSize(volatile Fifo_t fifo) {
    uint32_t size;

    if(fifo->frontIdx == fifo->backIdx) {                                      // empty
        size = 0;
    }
    else if(((fifo->backIdx + 1) % fifo->N) == fifo->frontIdx) {               // full
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
