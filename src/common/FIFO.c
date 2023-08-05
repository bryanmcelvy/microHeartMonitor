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
    uint16_t * const start_ptr;
    const uint16_t N;
    uint16_t read_idx;
    uint16_t write_idx;
};

FIFO_t FIFO_Init(uint16_t N) {
    uint16_t buffer[N];
    FIFO_t new_fifo = {buffer, N, 0, 0};
    return new_fifo;
}

uint16_t FIFO_Get(FIFO_t * fifo_ptr) {
    FIFO_t fifo = *fifo_ptr;
    uint16_t ret_val;

    if(fifo.read_idx == fifo.write_idx) {       // FIFO is empty
        ret_val = 0;
    }
    else {
        ret_val = fifo.start_ptr[fifo.read_idx];
        fifo.read_idx = (fifo.read_idx > 0) ? (fifo.read_idx - 1) : (fifo.N - 1);
    }

    return ret_val;
}

void FIFO_Add(FIFO_t * fifo_ptr, uint16_t val) {
    FIFO_t fifo = *fifo_ptr;

    fifo.start_ptr[fifo.write_idx++] = val;
    fifo.write_idx = (fifo.write_idx < fifo.N) ? fifo.write_idx : 0;
}

/** @} */
