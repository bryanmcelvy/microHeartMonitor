/******************************************************************************
Preprocessor Directives
*******************************************************************************/
#include "GPIO_New.h"

#include "tm4c123gh6pm.h"

#include <NewAssert.h>
#include <stdbool.h>
#include <stdint.h>

#define GPIO_NUM_PORTS 6

// Base Addresses
enum {
    GPIO_PORTA_BASE_ADDRESS = (uint32_t) 0x40004000,
    GPIO_PORTB_BASE_ADDRESS = (uint32_t) 0x40005000,
    GPIO_PORTC_BASE_ADDRESS = (uint32_t) 0x40006000,
    GPIO_PORTD_BASE_ADDRESS = (uint32_t) 0x40007000,
    GPIO_PORTE_BASE_ADDRESS = (uint32_t) 0x40024000,
    GPIO_PORTF_BASE_ADDRESS = (uint32_t) 0x40025000,
};

// Register Offsets
enum {
    GPIO_DATA_R_OFFSET = (uint32_t) 0x03FC,
    GPIO_DIR_R_OFFSET = (uint32_t) 0x0400,
    GPIO_IS_R_OFFSET = (uint32_t) 0x0404,
    GPIO_IBE_R_OFFSET = (uint32_t) 0x0408,
    GPIO_IEV_R_OFFSET = (uint32_t) 0x040C,
    GPIO_IM_R_OFFSET = (uint32_t) 0x0410,
    GPIO_ICR_R_OFFSET = (uint32_t) 0x041C,
    GPIO_AFSEL_R_OFFSET = (uint32_t) 0x0420,
    GPIO_DR2R_R_OFFSET = (uint32_t) 0x0500,
    GPIO_DR4R_R_OFFSET = (uint32_t) 0x0504,
    GPIO_DR8R_R_OFFSET = (uint32_t) 0x0508,
    GPIO_PUR_R_OFFSET = (uint32_t) 0x0510,
    GPIO_PDR_R_OFFSET = (uint32_t) 0x0518,
    GPIO_DEN_R_OFFSET = (uint32_t) 0x051C,
    GPIO_LOCK_R_OFFSET = (uint32_t) 0x0520,
    GPIO_COMMIT_R_OFFSET = (uint32_t) 0x0524,
    GPIO_AMSEL_R_OFFSET = (uint32_t) 0x0528,
    GPIO_PCTL_R_OFFSET = (uint32_t) 0x052C
};

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
GPIO_Port_t * GPIO_InitPort(GPIO_PortName_t portName) {
    Assert(portName < GPIO_NUM_PORTS);

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

bool GPIO_isPortInit(GPIO_Port_t * gpioPort) {
    return gpioPort->isInit;
}

/******************************************************************************
Configuration (Digital I/O)
*******************************************************************************/

void GPIO_ConfigDirOutput(GPIO_Port_t * gpioPort, uint8_t bitMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_DIR_R_OFFSET)) |= bitMask;
    return;
}

void GPIO_ConfigDirInput(GPIO_Port_t * gpioPort, uint8_t bitMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_DIR_R_OFFSET)) &= ~(bitMask);
    return;
}

void GPIO_ConfigPullUp(GPIO_Port_t * gpioPort, uint8_t bitMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_PUR_R_OFFSET)) |= bitMask;
    return;
}

void GPIO_ConfigPullDown(GPIO_Port_t * gpioPort, uint8_t bitMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_PDR_R_OFFSET)) |= bitMask;
    return;
}

void GPIO_ConfigDriveSelect(GPIO_Port_t * gpioPort, uint8_t bitMask, uint8_t drive_mA) {
    uint32_t driveSelectRegister_Offset;
    switch(drive_mA) {
        case 2: driveSelectRegister_Offset = GPIO_DR2R_R_OFFSET; break;
        case 4: driveSelectRegister_Offset = GPIO_DR4R_R_OFFSET; break;
        case 8: driveSelectRegister_Offset = GPIO_DR8R_R_OFFSET; break;
        default: driveSelectRegister_Offset = 0; Assert(false);
    }
    *((register_t) (gpioPort->BASE_ADDRESS + driveSelectRegister_Offset)) |= bitMask;
    return;
}

void GPIO_EnableDigital(GPIO_Port_t * gpioPort, uint8_t bitMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_DEN_R_OFFSET)) |= bitMask;
    return;
}

void GPIO_DisableDigital(GPIO_Port_t * gpioPort, uint8_t bitMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_DEN_R_OFFSET)) &= ~bitMask;
    return;
}

/******************************************************************************
Basic Functions (Digital I/O)
*******************************************************************************/

uint8_t GPIO_ReadPins(GPIO_Port_t * gpioPort, uint8_t bitMask) {
    return *((register_t) (gpioPort->BASE_ADDRESS + GPIO_DATA_R_OFFSET)) & bitMask;
}

void GPIO_WriteHigh(GPIO_Port_t * gpioPort, uint8_t bitMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_DATA_R_OFFSET)) |= bitMask;
    return;
}

void GPIO_WriteLow(GPIO_Port_t * gpioPort, uint8_t bitMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_DATA_R_OFFSET)) &= ~(bitMask);
    return;
}

void GPIO_Toggle(GPIO_Port_t * gpioPort, uint8_t bitMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_DATA_R_OFFSET)) ^= bitMask;
    return;
}

/******************************************************************************
Configuration (Alternate/Analog Modes)
*******************************************************************************/

void GPIO_ConfigAltMode(GPIO_Port_t * gpioPort, uint8_t bitMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_AFSEL_R_OFFSET)) |= bitMask;
    return;
}

void GPIO_ConfigPortCtrl(GPIO_Port_t * gpioPort, uint8_t bitMask, uint8_t fieldEncoding) {
    // TODO: Write explanation
    register_t pctl_register = (register_t) (gpioPort->BASE_ADDRESS + GPIO_PCTL_R_OFFSET);
    for(uint8_t i = 0; i < 8; i++) {
        if(bitMask & (1 << i)) {
            *pctl_register |= (fieldEncoding << (4 * i));
        }
    }
}

void GPIO_ConfigAnalog(GPIO_Port_t * gpioPort, uint8_t bitMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_AMSEL_R_OFFSET)) |= bitMask;
    return;
}
