#include "fifo_buff.h"

#define BUFFER_SIZE (uint32_t) 10

int main() {
    FIFO_buffer_t test_FIFO = FIFO_init(BUFFER_SIZE);
    FIFO_show_data(&test_FIFO);

    for (int i = 0; i < BUFFER_SIZE; i++) {
        FIFO_add_sample(&test_FIFO, i);
        FIFO_show_data(&test_FIFO);
    }

    for (int i = 0; i < BUFFER_SIZE; i++) {
        FIFO_rem_sample(&test_FIFO);
        FIFO_show_data(&test_FIFO);
    }

    return 0;
}