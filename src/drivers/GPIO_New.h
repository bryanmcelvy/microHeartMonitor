#include "tm4c123gh6pm.h"
#include "stdint.h"
#include "stdbool.h"

enum { A, B, C, D, E, F } P0RT_NAMES;

typedef struct GPIO_t GPIO_t;

/**********************************************************************
Initialization
***********************************************************************/

GPIO_t * GPIO_PortInit(uint8_t portName);

/**********************************************************************
Configuration
***********************************************************************/

void GPIO_SetDirToOut(GPIO_t * gpioPort, uint8_t bitMask);

/**********************************************************************
Basic Functions
***********************************************************************/

void GPIO_Write_HIGH(GPIO_t * gpioPort, uint8_t bitMask);
