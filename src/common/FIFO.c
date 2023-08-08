/**
 * @addtogroup fifo
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for FIFO buffer module.
 */

#include "FIFO.h"

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#ifndef FIFO_POOL_SIZE
#define FIFO_POOL_SIZE 3
#endif

struct FIFO_t {
    uint16_t * buffer;                           ///< (pointer to) array to use as FIFO buffer
    uint16_t N;                                  ///< length of `buffer`
    uint16_t front_idx;                          ///< idx of front of FIFO
    uint16_t back_idx;                           ///< idx of back of FIFO
};

static FIFO_t buffer_pool[FIFO_POOL_SIZE] = { 0 };
static uint8_t free_buffers = FIFO_POOL_SIZE;

FIFO_t * FIFO_Init(uint16_t buffer[], uint16_t N) {
    FIFO_t * fifo_ptr = 0;
    if(free_buffers > 0) {
        free_buffers -= 1;
        fifo_ptr = &(buffer_pool[free_buffers]);

        fifo_ptr->buffer = buffer;
        fifo_ptr->N = N;
        fifo_ptr->front_idx = 0;
        fifo_ptr->back_idx = 0;
    }
    assert(fifo_ptr);

    return fifo_ptr;
}

void FIFO_Put(FIFO_t * fifo_ptr, uint16_t val) {
    if((fifo_ptr->back_idx + 1) % 10 !=
       fifo_ptr->front_idx) {                    // ensure FIFO isn't full
        fifo_ptr->buffer[fifo_ptr->back_idx] = val;
        fifo_ptr->back_idx = (fifo_ptr->back_idx + 1) %
                             fifo_ptr->N;                     // modulo causes wrap around to 0
    }
}

uint16_t FIFO_Get(FIFO_t * fifo_ptr) {
    uint16_t ret_val;

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

void FIFO_Flush(FIFO_t * fifo_ptr, uint16_t output_buffer[]) {
    uint16_t idx = 0;

    while(FIFO_isEmpty(fifo_ptr) == false) {
        output_buffer[idx++] = fifo_ptr->buffer[fifo_ptr->front_idx];
        fifo_ptr->front_idx =
            (fifo_ptr->front_idx + 1) % fifo_ptr->N;                    // wrap around to end
    }
}

void FIFO_Peek(FIFO_t * fifo_ptr, uint16_t output_buffer[]) {
    uint16_t temp_front_idx = fifo_ptr->front_idx;
    uint16_t idx = 0;

    while((temp_front_idx != fifo_ptr->back_idx)) {
        output_buffer[idx++] = fifo_ptr->buffer[temp_front_idx];
        temp_front_idx =
            (temp_front_idx + 1) % fifo_ptr->N;                         // wrap around to end
    }
}

bool FIFO_isFull(FIFO_t * fifo_ptr) {
    return ((fifo_ptr->back_idx + 1) % fifo_ptr->N) == fifo_ptr->front_idx;
}

bool FIFO_isEmpty(FIFO_t * fifo_ptr) { return fifo_ptr->front_idx == fifo_ptr->back_idx; }

uint16_t FIFO_getCurrSize(FIFO_t * fifo_ptr) {
    uint16_t size;

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
