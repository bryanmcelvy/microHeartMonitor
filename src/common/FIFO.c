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
    uint16_t * start_ptr;
    uint16_t N;
    uint16_t read_idx;
    uint16_t write_idx;
};

FIFO_t * FIFO_Init(FIFO_t * fifo_ptr, uint16_t buffer[], uint16_t N) {
    fifo_ptr->start_ptr = buffer;
    fifo_ptr->N = N;
    fifo_ptr->read_idx = 0;
    fifo_ptr->write_idx = 0;

    return fifo_ptr;
}

uint16_t FIFO_Get(FIFO_t * fifo_ptr) {
    uint16_t ret_val;

    if(fifo_ptr->read_idx != fifo_ptr->write_idx) {
        ret_val = fifo_ptr->start_ptr[fifo_ptr->read_idx];
        fifo_ptr->read_idx = (fifo_ptr->read_idx + 1) % fifo_ptr->N;            // wrap around to end
    }
    else {                                                                      // FIFO is empty
        ret_val = 0;
    }

    return ret_val;
}

void FIFO_Put(FIFO_t * fifo_ptr, uint16_t val) {

    if( (fifo_ptr->write_idx + 1) % 10 != fifo_ptr->read_idx ) {                // ensure FIFO isn't full
        fifo_ptr->start_ptr[fifo_ptr->write_idx] = val;
        fifo_ptr->write_idx = (fifo_ptr->write_idx + 1) % fifo_ptr->N;
    }

}

/** @} */
