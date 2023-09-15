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

struct FIFO_t {
    volatile uint32_t * buffer;                ///< (pointer to) array to use as FIFO buffer
    volatile uint32_t N;                       ///< length of `buffer`
    volatile uint32_t front_idx;               ///< idx of front of FIFO
    volatile uint32_t back_idx;                ///< idx of back of FIFO
};

static FIFO_t buffer_pool[FIFO_POOL_SIZE] = { 0 };               ///< pre-allocated buffer pool
static uint8_t free_buffers = FIFO_POOL_SIZE;                    ///< no. of remaining buffers

volatile FIFO_t * FIFO_Init(volatile uint32_t buffer[], const uint32_t N) {
    /// TODO: Add details
    volatile FIFO_t * fifo_ptr = 0;

    Assert(free_buffers > 0);
    fifo_ptr = &(buffer_pool[--free_buffers]);
    fifo_ptr->buffer = buffer;
    fifo_ptr->N = N;
    fifo_ptr->front_idx = 0;
    fifo_ptr->back_idx = 0;

    return fifo_ptr;
}

/******************************************************************************
Basic Operations
*******************************************************************************/

void FIFO_Put(volatile FIFO_t * fifo_ptr, const uint32_t val) {
    // NOTE: not using FIFO_isFull() here to reduce call stack usage
    if(((fifo_ptr->back_idx + 1) % fifo_ptr->N) != fifo_ptr->front_idx) {
        fifo_ptr->buffer[fifo_ptr->back_idx] = val;
        fifo_ptr->back_idx =
            (fifo_ptr->back_idx + 1) % fifo_ptr->N;               // modulo causes wrap around to 0
    }
}

uint32_t FIFO_Get(volatile FIFO_t * fifo_ptr) {
    volatile uint32_t ret_val;

    // NOTE: not using FIFO_isEmpty() here to reduce call stack usage
    if(fifo_ptr->front_idx == fifo_ptr->back_idx) {
        ret_val = 0;
    }
    else {
        ret_val = fifo_ptr->buffer[fifo_ptr->front_idx];
        fifo_ptr->front_idx =
            (fifo_ptr->front_idx + 1) % fifo_ptr->N;               // modulo causes wrap around to 0
    }

    return ret_val;
}

void FIFO_TransferOne(volatile FIFO_t * src_fifo_ptr, volatile FIFO_t * dest_fifo_ptr) {
    if((FIFO_isEmpty(src_fifo_ptr) == false) && (FIFO_isFull(dest_fifo_ptr) == false)) {
        FIFO_Put(dest_fifo_ptr, FIFO_Get(src_fifo_ptr));
    }
}

/******************************************************************************
Bulk Removal
*******************************************************************************/

void FIFO_Flush(volatile FIFO_t * fifo_ptr, uint32_t output_buffer[]) {
    uint32_t idx = 0;

    while(FIFO_isEmpty(fifo_ptr) == false) {
        output_buffer[idx++] = fifo_ptr->buffer[fifo_ptr->front_idx];
        fifo_ptr->front_idx =
            (fifo_ptr->front_idx + 1) % fifo_ptr->N;               // wrap around to end
    }
}

void FIFO_Reset(volatile FIFO_t * fifo_ptr) {
    fifo_ptr->back_idx = fifo_ptr->front_idx;
    return;
}

void FIFO_TransferAll(volatile FIFO_t * src_fifo_ptr, volatile FIFO_t * dest_fifo_ptr) {
    while((FIFO_isEmpty(src_fifo_ptr) == false) && (FIFO_isFull(dest_fifo_ptr) == false)) {
        FIFO_Put(dest_fifo_ptr, FIFO_Get(src_fifo_ptr));
    }
}

/******************************************************************************
Peeking
*******************************************************************************/

uint32_t FIFO_PeekOne(volatile FIFO_t * fifo_ptr) {
    uint32_t ret_val;

    if(FIFO_isEmpty(fifo_ptr)) {
        ret_val = 0;
    }
    else {
        ret_val = fifo_ptr->buffer[fifo_ptr->front_idx];
    }

    return ret_val;
}

void FIFO_PeekAll(volatile FIFO_t * fifo_ptr, uint32_t output_buffer[]) {
    uint32_t temp_front_idx = fifo_ptr->front_idx;
    uint32_t idx = 0;

    while(temp_front_idx != fifo_ptr->back_idx) {
        output_buffer[idx++] = fifo_ptr->buffer[temp_front_idx];
        temp_front_idx = (temp_front_idx + 1) % fifo_ptr->N;               // wrap around to end
    }
}

/******************************************************************************
Status Checks
*******************************************************************************/

bool FIFO_isFull(volatile FIFO_t * fifo_ptr) {
    return (bool) (((fifo_ptr->back_idx + 1) % fifo_ptr->N) == fifo_ptr->front_idx);
}

bool FIFO_isEmpty(volatile FIFO_t * fifo_ptr) {
    return (bool) (fifo_ptr->front_idx == fifo_ptr->back_idx);
}

uint32_t FIFO_getCurrSize(volatile FIFO_t * fifo_ptr) {
    uint32_t size;

    if(FIFO_isEmpty(fifo_ptr)) {
        size = 0;
    }
    else if(FIFO_isFull(fifo_ptr)) {
        size = fifo_ptr->N - 1;
    }
    else if(fifo_ptr->front_idx < fifo_ptr->back_idx) {
        size = fifo_ptr->back_idx - fifo_ptr->front_idx;
    }
    else {
        size = fifo_ptr->N - (fifo_ptr->front_idx - fifo_ptr->back_idx);
    }

    return size;
}

/** @} */
