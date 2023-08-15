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
        Status Checks
*******************************************************************************/

/******************************************************************************
Preprocessor Directives
*******************************************************************************/

#include "FIFO.h"

#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
Type Declaration + Initialization
*******************************************************************************/

struct FIFO_t {
    uint32_t * buffer;                     ///< (pointer to) array to use as FIFO buffer
    uint32_t N;                            ///< length of `buffer`
    uint32_t front_idx;                    ///< idx of front of FIFO
    uint32_t back_idx;                     ///< idx of back of FIFO
};

static FIFO_t buffer_pool[FIFO_POOL_SIZE] = { 0 };                    ///< pre-allocated buffer pool
static uint8_t free_buffers = FIFO_POOL_SIZE;                         ///< no. of remaining buffers

FIFO_t * FIFO_Init(uint32_t buffer[], uint32_t N) {
    /// TODO: Add details
    FIFO_t * fifo_ptr = 0;

    if(free_buffers > 0) {
        fifo_ptr = &(buffer_pool[--free_buffers]);
        fifo_ptr->buffer = buffer;
        fifo_ptr->N = N;
        fifo_ptr->front_idx = 0;
        fifo_ptr->back_idx = 0;
    }

    return fifo_ptr;
}

/******************************************************************************
Basic Operations
*******************************************************************************/

void FIFO_Put(FIFO_t * fifo_ptr, uint32_t val) {
    if(FIFO_isFull(fifo_ptr) == false) {                      // ensure FIFO isn't full
        fifo_ptr->buffer[fifo_ptr->back_idx] = val;
        fifo_ptr->back_idx = (fifo_ptr->back_idx + 1) %
                             fifo_ptr->N;                     // modulo causes wrap around to 0
    }
}

uint32_t FIFO_Get(FIFO_t * fifo_ptr) {
    uint32_t ret_val;

    if(FIFO_isEmpty(fifo_ptr)) {
        ret_val = 0;
    }
    else {
        ret_val = fifo_ptr->buffer[fifo_ptr->front_idx];
        fifo_ptr->front_idx = (fifo_ptr->front_idx + 1) %
                              fifo_ptr->N;                    // modulo causes wrap around to 0
    }

    return ret_val;
}

void FIFO_TransferOne(FIFO_t * src_fifo_ptr, FIFO_t * dest_fifo_ptr) {
    if((FIFO_isEmpty(src_fifo_ptr) == false) && (FIFO_isFull(dest_fifo_ptr) == false)) {
        FIFO_Put(dest_fifo_ptr, FIFO_Get(src_fifo_ptr));
    }
}

/******************************************************************************
Bulk Removal
*******************************************************************************/

void FIFO_Flush(FIFO_t * fifo_ptr, uint32_t output_buffer[]) {
    uint32_t idx = 0;

    while(FIFO_isEmpty(fifo_ptr) == false) {
        output_buffer[idx++] = fifo_ptr->buffer[fifo_ptr->front_idx];
        fifo_ptr->front_idx =
            (fifo_ptr->front_idx + 1) % fifo_ptr->N;                    // wrap around to end
    }
}

void FIFO_TransferAll(FIFO_t * src_fifo_ptr, FIFO_t * dest_fifo_ptr) {
    while((FIFO_isEmpty(src_fifo_ptr) == false) && (FIFO_isFull(dest_fifo_ptr) == false)) {
        FIFO_Put(dest_fifo_ptr, FIFO_Get(src_fifo_ptr));
    }
}

/******************************************************************************
Status Checks
*******************************************************************************/

uint32_t FIFO_PeekOne(FIFO_t * fifo_ptr) {
    uint32_t ret_val;

    if(FIFO_isEmpty(fifo_ptr)) {
        ret_val = 0;
    }
    else {
        ret_val = fifo_ptr->buffer[fifo_ptr->front_idx];
    }

    return ret_val;
}

void FIFO_PeekAll(FIFO_t * fifo_ptr, uint32_t output_buffer[]) {
    uint32_t temp_front_idx = fifo_ptr->front_idx;
    uint32_t idx = 0;

    while(temp_front_idx != fifo_ptr->back_idx) {
        output_buffer[idx++] = fifo_ptr->buffer[temp_front_idx];
        temp_front_idx =
            (temp_front_idx + 1) % fifo_ptr->N;                    // wrap around to end
    }
}

bool FIFO_isFull(FIFO_t * fifo_ptr) {
    return ((fifo_ptr->back_idx + 1) % fifo_ptr->N) == fifo_ptr->front_idx;
}

bool FIFO_isEmpty(FIFO_t * fifo_ptr) {
    return fifo_ptr->front_idx == fifo_ptr->back_idx;
}

uint32_t FIFO_getCurrSize(FIFO_t * fifo_ptr) {
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
