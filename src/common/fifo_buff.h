/**
 * @file    fifo_buff.h
 * @author  Bryan McElvy
 * @brief   Header file for FIFO buffer type
 * 
 */

#include <stdint.h>
#include <stdio.h>

/**
 * @brief   Array-based FIFO buffer type.
 */
typedef struct FIFO_buffer FIFO_buffer_t;

/**
 * @brief Initializes a FIFO buffer with the specified size.
 * 
 * @param buffer_size   desired buffer size.
 * @return              FIFO_buffer
 */
FIFO_buffer_t FIFO_init(uint32_t buffer_size);

/**
 * @brief Adds a 16-bit sample to the end of the FIFO buffer at the specified address.
 * 
 * @param FIFO_buffer   pointer to FIFO buffer
 * @param sample        data sample to be added
 * @return              None 
 */
void FIFO_add_sample(FIFO_buffer_t *FIFO_ptr, uint16_t sample);

/**
 * @brief Removes the first element of the FIFO buffer at the specified address.
 * 
 * @param FIFO_ptr      pointer to FIFO buffer
 * @return              uint16_t 
 */
uint16_t FIFO_rem_sample(FIFO_buffer_t *FIFO_ptr);

/**
 * @brief Gets the size of the FIFO buffer at the specified address.
 * 
 * @param FIFO_ptr      pointer to FIFO buffer
 * @return              curr_size 
 */
uint32_t FIFO_get_size(FIFO_buffer_t *FIFO_ptr);

/**
 * @brief               Shows all of the items in the FIFO buffer at the specified address.
 *                      NOTE: Intended for debugging purposes only.
 * 
 * @param FIFO_ptr      pointer to FIFO buffer
 */
void FIFO_show_data(FIFO_buffer_t *FIFO_ptr);
