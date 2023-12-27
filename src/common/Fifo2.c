#include "Fifo2.h"

#include "NewAssert.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/******************************************************************************
SECTIONS
        Initialization
        Basic Operations (Int)
        Basic Operations (Float)
        Peeking
        Status Checks
*******************************************************************************/

static void copyData(void * source, void * dest, const size_t N) {
    uint8_t * srcPtr = source;
    uint8_t * destPtr = dest;
    for(uint32_t n = 0; n < N; n++) {
        *destPtr++ = *srcPtr++;
    }
    return;
}

/******************************************************************************
Initialization
*******************************************************************************/

typedef struct FifoStruct_t {
    volatile uint32_t N;                      ///< length of `buffer`
    volatile size_t dataSize;
    volatile uint32_t frontIdx;               ///< idx of front of FIFO
    volatile uint32_t backIdx;                ///< idx of back of FIFO
    volatile uint32_t * buffer;               ///< (pointer to) array to use as FIFO buffer
} FifoStruct_t;

static FifoStruct_t fifoPool[FIFO_POOL_SIZE] = { 0 };               ///< pre-allocated pool
static uint8_t numFreeFifos = FIFO_POOL_SIZE;

Fifo_t Fifo_Init(volatile uint32_t buffer[], const uint32_t N, size_t dataSize) {
    assert(numFreeFifos > 0);

    numFreeFifos -= 1;
    volatile Fifo_t fifo = &(fifoPool[numFreeFifos]);

    fifo->N = N;
    fifo->dataSize = dataSize;
    fifo->frontIdx = 0;
    fifo->backIdx = 0;
    fifo->buffer = buffer;

    return fifo;
}

void Fifo_Reset(volatile Fifo_t fifo) {
    fifo->backIdx = fifo->frontIdx;
    return;
}

/******************************************************************************
Basic Operations
*******************************************************************************/

#define UPDATE_FRONT_IDX(F) (((F)->frontIdx + 1) % (F)->N)
#define UPDATE_BACK_IDX(F)  (((F)->backIdx + 1) % (F)->N)

FifoStatus_e Fifo_Put(volatile Fifo_t fifo, void * inputValPtr) {
    if(((fifo->backIdx + 1) % fifo->N) == fifo->frontIdx) {
        return FIFO_FAILURE;
    }
    else {
        uint8_t * destPtr = ((uint8_t *) fifo->buffer) + (fifo->backIdx * fifo->dataSize);
        copyData(inputValPtr, destPtr, fifo->dataSize);
        fifo->backIdx = UPDATE_BACK_IDX(fifo);
        return FIFO_SUCCESS;
    }
}

FifoStatus_e Fifo_Get(volatile Fifo_t fifo, void * outputValPtr) {
    if(fifo->frontIdx == fifo->backIdx) {
        return FIFO_FAILURE;
    }
    else {
        uint8_t * sourcePtr = ((uint8_t *) fifo->buffer) + (fifo->frontIdx * fifo->dataSize);
        copyData(sourcePtr, outputValPtr, fifo->dataSize);
        fifo->frontIdx = UPDATE_FRONT_IDX(fifo);
        return FIFO_SUCCESS;
    }
}

#define GET_ARRAY_LEN(A) ((int) (sizeof((A)) / sizeof((A)[0])))

FifoStatus_e Fifo_Flush(volatile Fifo_t fifo, void * outputBuffer) {
    if(GET_ARRAY_LEN(outputBuffer) != fifo->N) {
        return FIFO_FAILURE;
    }
    else {
        uint8_t * destPtr = outputBuffer;
        while(fifo->frontIdx != fifo->backIdx) {
            uint8_t * sourcePtr = ((uint8_t *) fifo->buffer) + (fifo->frontIdx * fifo->dataSize);
            copyData(sourcePtr, destPtr, fifo->dataSize);

            fifo->frontIdx = UPDATE_FRONT_IDX(fifo);
            destPtr += (fifo->dataSize);
        }
        return FIFO_SUCCESS;
    }
}

/******************************************************************************
Peeking
*******************************************************************************/

FifoStatus_e Fifo_PeekOne(volatile Fifo_t fifo, void * outputValPtr) {
    if(fifo->frontIdx == fifo->backIdx) {
        return FIFO_FAILURE;
    }
    else {
        uint8_t * sourcePtr = ((uint8_t *) fifo->buffer) + (fifo->frontIdx * fifo->dataSize);
        copyData(sourcePtr, outputValPtr, fifo->dataSize);
        return FIFO_SUCCESS;
    }
}

FifoStatus_e Fifo_PeekAll(volatile Fifo_t fifo, void * outputBuffer) {
    if(GET_ARRAY_LEN(outputBuffer) != fifo->N) {
        return FIFO_FAILURE;
    }
    else {
        uint8_t * destPtr = outputBuffer;
        uint32_t frontIdx = fifo->frontIdx;
        while(fifo->frontIdx != fifo->backIdx) {
            uint8_t * sourcePtr = ((uint8_t *) fifo->buffer) + (fifo->frontIdx * fifo->dataSize);
            copyData(sourcePtr, destPtr, fifo->dataSize);

            frontIdx = (frontIdx + 1) % fifo->N;
            destPtr += (fifo->dataSize);
        }
        return FIFO_SUCCESS;
    }
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
    if(fifo->frontIdx == fifo->backIdx) {                                      // empty
        return 0;
    }
    else if(((fifo->backIdx + 1) % fifo->N) == fifo->frontIdx) {               // full
        return fifo->N - 1;
    }
    else if(fifo->frontIdx < fifo->backIdx) {
        return (fifo->backIdx - fifo->frontIdx);
    }
    else {
        return (fifo->N - (fifo->frontIdx - fifo->backIdx));
    }
}

/** @} */
