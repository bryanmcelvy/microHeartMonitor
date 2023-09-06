/******************************************************************************
Preprocessor Directives
*******************************************************************************/
#include "GPIO_New.h"

#include "tm4c123gh6pm.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

// Base Addresses
#define GPIO_PORTA_BASE_ADDRESS (uint32_t) 0x40004000
#define GPIO_PORTB_BASE_ADDRESS (uint32_t) 0x40005000
#define GPIO_PORTC_BASE_ADDRESS (uint32_t) 0x40006000
#define GPIO_PORTD_BASE_ADDRESS (uint32_t) 0x40007000
#define GPIO_PORTE_BASE_ADDRESS (uint32_t) 0x40024000
#define GPIO_PORTF_BASE_ADDRESS (uint32_t) 0x40025000

// Offsets
#define GPIO_DATA_R_OFFSET      (uint32_t) 0x0000
#define GPIO_DIR_R_OFFSET       (uint32_t) 0x0400
#define GPIO_IS_R_OFFSET        (uint32_t) 0x0404
#define GPIO_IBE_R_OFFSET       (uint32_t) 0x0408
#define GPIO_IEV_R_OFFSET       (uint32_t) 0x040C
#define GPIO_IM_R_OFFSET        (uint32_t) 0x0410
#define GPIO_ICR_R_OFFSET       (uint32_t) 0x041C
#define GPIO_AFSEL_R_OFFSET     (uint32_t) 0x0420
#define GPIO_DR2R_R_OFFSET      (uint32_t) 0x0500
#define GPIO_DR4R_R_OFFSET      (uint32_t) 0x0504
#define GPIO_DR8R_R_OFFSET      (uint32_t) 0x0508
#define GPIO_PUR_R_OFFSET       (uint32_t) 0x0510
#define GPIO_PDR_R_OFFSET       (uint32_t) 0x0518
#define GPIO_DEN_R_OFFSET       (uint32_t) 0x051C
#define GPIO_LOCK_R_OFFSET      (uint32_t) 0x0520
#define GPIO_COMMIT_R_OFFSET    (uint32_t) 0x0524
#define GPIO_AMSEL_R_OFFSET     (uint32_t) 0x0528
#define GPIO_PCTL_R_OFFSET      (uint32_t) 0x052C

/******************************************************************************
Struct Definition
*******************************************************************************/
typedef volatile uint32_t * register_t;

struct GPIO_Port_t {
    const uint32_t BASE_ADDRESS;
    bool isInit;
};

static GPIO_Port_t GPIO_PTR_ARR[6] = {
    { GPIO_PORTA_BASE_ADDRESS, false }, { GPIO_PORTB_BASE_ADDRESS, false },
    { GPIO_PORTC_BASE_ADDRESS, false }, { GPIO_PORTD_BASE_ADDRESS, false },
    { GPIO_PORTE_BASE_ADDRESS, false }, { GPIO_PORTF_BASE_ADDRESS, false },
};

/******************************************************************************
Initialization
*******************************************************************************/
GPIO_Port_t * GPIO_PortInit(GPIO_PortName_t portName) {
    assert(portName < 5);

    GPIO_Port_t * gpio_ptr = &GPIO_PTR_ARR[portName];
    if(gpio_ptr->isInit == false) {
        // Start clock for port and wait for it to be ready
        SYSCTL_RCGCGPIO_R |= (1 << portName);
        while((SYSCTL_PRGPIO_R & (1 << portName)) == 0) {}

        // Disable alternate and analog modes
        *((register_t) (gpio_ptr->BASE_ADDRESS + GPIO_AMSEL_R_OFFSET)) &= ~(0xFF);
        *((register_t) (gpio_ptr->BASE_ADDRESS + GPIO_AFSEL_R_OFFSET)) &= ~(0xFF);
        *((register_t) (gpio_ptr->BASE_ADDRESS + GPIO_PCTL_R_OFFSET)) &= ~(0xFF);

        gpio_ptr->isInit = true;
    }

    return gpio_ptr;
}

/******************************************************************************
Configuration (Digital I/O)
*******************************************************************************/

void GPIO_ConfigDir(GPIO_Port_t * gpioPort, uint8_t bitMask) {
    // *(gpioPort->DIR_R) |= bitMask;
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_DIR_R_OFFSET)) |= bitMask;
}

void GPIO_ConfigPullUp(GPIO_Port_t * gpioPort, uint8_t bitMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_PUR_R_OFFSET)) |= bitMask;
}

void GPIO_ConfigPullDown(GPIO_Port_t * gpioPort, uint8_t bitMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_PDR_R_OFFSET)) |= bitMask;
}

void GPIO_ConfigDriveSelect(GPIO_Port_t * gpioPort, uint8_t bitMask, uint8_t drive_mA) {
    uint32_t driveSelectRegister_Offset;
    switch(drive_mA) {
        case 2: driveSelectRegister_Offset = GPIO_DR2R_R_OFFSET; break;
        case 4: driveSelectRegister_Offset = GPIO_DR4R_R_OFFSET; break;
        case 8: driveSelectRegister_Offset = GPIO_DR8R_R_OFFSET; break;
        default: assert(false);
    }
    *((register_t) (gpioPort->BASE_ADDRESS + driveSelectRegister_Offset)) |= bitMask;
}

void GPIO_EnableDigital(GPIO_Port_t * gpioPort, uint8_t bitMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_DEN_R_OFFSET)) |= bitMask;
}

/******************************************************************************
Basic Functions (Digital I/O)
*******************************************************************************/

void GPIO_WriteHigh(GPIO_Port_t * gpioPort, uint8_t bitMask) {

    *((register_t) gpioPort->BASE_ADDRESS) |= bitMask;
}

void GPIO_WriteLow(GPIO_Port_t * gpioPort, uint8_t bitMask) {

    *((register_t) gpioPort->BASE_ADDRESS) &= ~(bitMask);
}
