/**
 * @file    fifo_buff.c
 * @author  Bryan McElvy
 * @brief   Source code file for FIFO buffer type
 * 
 */

#include "fifo_buff.h"

/**
 * @param front_ptr     pointer to the first element of the buffer.
 * @param rear_ptr      pointer to the last element of the buffer.
 * @param curr_size     current number of elements within the buffer.
 * @param MAX_SIZE      maximum number of elements allowed within buffer.
 */
struct FIFO_buffer {
    volatile uint16_t * front_ptr;
    volatile uint16_t * rear_ptr;
    volatile uint32_t   curr_size;
    uint32_t            MAX_SIZE;
};

FIFO_buffer_t FIFO_init(uint32_t buffer_size) {
    /* Init. array with inputted size */
    volatile uint16_t buffer_array[buffer_size];

    /* Init. buffer and assign parameters */
    FIFO_buffer_t FIFO_buffer;
    
    // front/rear ptrs are both for index zero since the array is empty
    FIFO_buffer.front_ptr = &(buffer_array[0]);
    FIFO_buffer.rear_ptr = &(buffer_array[0]);
    FIFO_buffer.curr_size = 0;
    FIFO_buffer.MAX_SIZE = buffer_size;

    return FIFO_buffer;
}

void FIFO_add_sample(FIFO_buffer_t *FIFO_ptr, uint16_t sample) {
    if (FIFO_ptr->curr_size < FIFO_ptr->MAX_SIZE) {
        *(FIFO_ptr->rear_ptr) = sample;
        FIFO_ptr->rear_ptr += 1;
        FIFO_ptr->curr_size += 1;
    }
}

uint16_t FIFO_rem_sample(FIFO_buffer_t *FIFO_ptr)  {
    uint16_t front_val;
    
    if (FIFO_ptr->curr_size > 0) {
        front_val = *(FIFO_ptr->front_ptr);
        
        // Shift remaining values up by one
        volatile uint16_t * i_ptr = FIFO_ptr->front_ptr;
        for (int j = 0; j < (FIFO_ptr->curr_size - 1); j++) {
            *(i_ptr) = *(i_ptr + 1);
            i_ptr += 1;
        }
        
        FIFO_ptr->rear_ptr -= 1;
        FIFO_ptr->curr_size -= 1;
    }
    else {
        front_val = 0;
        // printf("FIFO is empty.");
    }

    return front_val;
}

uint32_t FIFO_get_size(FIFO_buffer_t *FIFO_ptr) {
    return FIFO_ptr->curr_size;
}

void FIFO_show_data(FIFO_buffer_t *FIFO_ptr) {
    if (FIFO_ptr->curr_size == 0) {
        // printf("FIFO is empty.\n");
    }
    else {
        for (int i = 0; i < FIFO_ptr->curr_size; i++) {
            // printf("%d ", *(FIFO_ptr->front_ptr + i));
        }
        // printf("\n");
    }
}
