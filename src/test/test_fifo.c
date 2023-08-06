/**
 * @file
 * @author  Bryan McElvy
 * @brief   Test script for FIFO buffer.
 */

#include "FIFO.h"

#include <stdint.h>
#include <stdlib.h>

#define FIFO_LEN 10

int main(void) {
    uint16_t buffer[FIFO_LEN];
    static FIFO_ptr_t fifo_ptr;

    srand(42);

    // Initialize FIFO buffer
    FIFO_Init(fifo_ptr, buffer, FIFO_LEN);

    // Add random numbers to the buffer
    for (int i = 0; i < FIFO_LEN; i++) {
        FIFO_Put(fifo_ptr, rand());
    }

}
