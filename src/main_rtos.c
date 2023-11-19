/**
 * @file
 * @author  Bryan McElvy
 * @brief   Main program file (RTOS implementation).
 */

// FreeRTOS
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

// application-specific
#include "DAQ.h"
#include "LCD.h"
#include "QRS.h"

// middleware
#include "Debug.h"

// common
#include "FIFO.h"

// drivers
#include "GPIO.h"
#include "ISR.h"
#include "PLL.h"
#include "UART.h"

// vendor (i.e. external/device) files
#include "arm_math_types.h"
#include "tm4c123gh6pm.h"

// other
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#define STACK_SIZE ((UBaseType_t) 100)

int main(void) {}

/** @} */               // main
