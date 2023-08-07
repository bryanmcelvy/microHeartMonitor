/**
 * @file
 * @author  Bryan McElvy
 * @brief   Test script for FIFO buffer.
 */

#include "FIFO.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define FIFO_LEN 10

int main(void) {
    uint16_t buffer[FIFO_LEN];
    FIFO_t * fifo_ptr = 0;
    uint16_t print_buffer[FIFO_LEN];
    uint16_t tmp_val;

    srand(42);

    // Initialize FIFO buffer
    fifo_ptr = FIFO_Init(buffer, FIFO_LEN);

    // Add random numbers to the buffer
    printf("Placing random values into buffer...");
    for (int i = 0; i < FIFO_LEN - 1; i++) {
        FIFO_Put(fifo_ptr, rand());
    }
    printf("done!\n");
    
    FIFO_Peek(fifo_ptr, print_buffer);
    for (int i = 0; i < FIFO_LEN - 1; i++) {
        printf("%d\n", print_buffer[i]);
    }

    // Remove one at a time
    printf("\nRemoving values...\n");
    for (int i = 0; i < FIFO_LEN - 1; i++) {
        tmp_val = FIFO_Get(fifo_ptr);
        printf("%d\n", tmp_val);
    }
    printf("...done!");
}

/*
cppcheck --clang=/usr/local/opt/llvm/bin/clang --enable=all -Isrc/common src/test/test_fifo.c src/common/FIFO.c
gcc -Wall -g -Isrc/common src/test/test_fifo.c src/common/FIFO.c -o build/src/test/test_fifo.o
chmod +x test_fifo_dir/test_fifo.o
./test_fifo_dir/test_fifo.o
*/
