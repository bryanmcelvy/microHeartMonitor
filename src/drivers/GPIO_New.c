#include "GPIO_New.h"

#include "tm4c123gh6pm.h"

#include <stdbool.h>
#include <stdint.h>

#define GPIO_PORTF_BASE_ADDRESS (volatile uint32_t *) 0x40025000

struct GPIO_t {
    volatile uint32_t * const DATA_R;
    volatile uint32_t * const DIR_R;
    volatile uint32_t * const INT_SENSE_R;
};
