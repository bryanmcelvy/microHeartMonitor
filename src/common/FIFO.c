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

struct FIFO_t { 
    uint16_t * buffer;      ///< (pointer to) array to use as FIFO buffer
    uint16_t N;             ///< length of `buffer`
    uint16_t front_idx;     ///< idx of front of FIFO
    uint16_t back_idx;      ///< idx of back of FIFO
};

FIFO_t * FIFO_Init(FIFO_t * fifo_ptr, uint16_t buffer[], uint16_t N) {
    fifo_ptr->buffer = buffer;
    fifo_ptr->N = N;
    fifo_ptr->front_idx = 0;
    fifo_ptr->back_idx = 0;

    return fifo_ptr;
}

void FIFO_Put(FIFO_t * fifo_ptr, uint16_t val) {
    if( (fifo_ptr->back_idx + 1) % 10 != fifo_ptr->front_idx ) {                // ensure FIFO isn't full
        fifo_ptr->buffer[fifo_ptr->back_idx] = val;
        fifo_ptr->back_idx = (fifo_ptr->back_idx + 1) % fifo_ptr->N;          // modulo causes wrap around to 0
    }
}

uint16_t FIFO_Get(FIFO_t * fifo_ptr) {
    uint16_t ret_val;

    if(fifo_ptr->front_idx != fifo_ptr->back_idx) {                             // ensure FIFO isn't empty
        ret_val = fifo_ptr->buffer[fifo_ptr->front_idx];
        fifo_ptr->front_idx = (fifo_ptr->front_idx + 1) % fifo_ptr->N;            // modulo causes wrap around to 0
    }
    else {
        ret_val = 0;
    }

    return ret_val;
}

void FIFO_Flush(FIFO_t * fifo_ptr, uint16_t output_buffer[]) {
    uint16_t idx = 0;
    
    while((fifo_ptr->front_idx != fifo_ptr->back_idx)) {
        output_buffer[idx++] = fifo_ptr->buffer[fifo_ptr->front_idx];
        fifo_ptr->front_idx = (fifo_ptr->front_idx + 1) % fifo_ptr->N;            // wrap around to end
    }
}

/** @} */
