/**
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for GPIO module.
 */

/******************************************************************************
Preprocessor Directives
*******************************************************************************/
#include "GPIO.h"

#include <NewAssert.h>

#include "tm4c123gh6pm.h"
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

        if(portName == F) {
            GPIO_PORTF_LOCK_R = 0x4C4F434B;               // Unlock GPIO Port F
            GPIO_PORTF_CR_R |= 0x01;                      // Allow changes to PF0
        }

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

void GPIO_ConfigDirOutput(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_DIR_R_OFFSET)) |= pinMask;
    return;
}

void GPIO_ConfigDirInput(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_DIR_R_OFFSET)) &= ~(pinMask);
    return;
}

void GPIO_ConfigPullUp(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_PUR_R_OFFSET)) |= pinMask;
    return;
}

void GPIO_ConfigPullDown(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_PDR_R_OFFSET)) |= pinMask;
    return;
}

void GPIO_ConfigDriveStrength(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask, uint8_t drive_mA) {
    uint32_t driveSelectRegister_Offset;
    switch(drive_mA) {
        case 2: driveSelectRegister_Offset = GPIO_DR2R_R_OFFSET; break;
        case 4: driveSelectRegister_Offset = GPIO_DR4R_R_OFFSET; break;
        case 8: driveSelectRegister_Offset = GPIO_DR8R_R_OFFSET; break;
        default: driveSelectRegister_Offset = 0; Assert(false);
    }
    *((register_t) (gpioPort->BASE_ADDRESS + driveSelectRegister_Offset)) |= pinMask;
    return;
}

void GPIO_EnableDigital(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_DEN_R_OFFSET)) |= pinMask;
    return;
}

void GPIO_DisableDigital(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_DEN_R_OFFSET)) &= ~pinMask;
    return;
}

/******************************************************************************
Configuration (Interrupts)
*******************************************************************************/

void GPIO_ConfigInterrupts_Edge(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask, bool risingEdge) {
    // Disable interrupts
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_IM_R_OFFSET)) &= ~(pinMask);

    // configure for edge-triggered interrupts
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_IS_R_OFFSET)) &= ~(pinMask);
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_IBE_R_OFFSET)) &= ~(pinMask);

    // select high or low edge
    if(risingEdge) {
        *((register_t) (gpioPort->BASE_ADDRESS + GPIO_IEV_R_OFFSET)) |= pinMask;
    }
    else {
        *((register_t) (gpioPort->BASE_ADDRESS + GPIO_IEV_R_OFFSET)) &= ~(pinMask);
    }

    // Clear interrupt flags and re-enable
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_ICR_R_OFFSET)) |= pinMask;
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_IM_R_OFFSET)) |= pinMask;

    return;
}

void GPIO_ConfigInterrupts_BothEdges(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    // Disable interrupts
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_IM_R_OFFSET)) &= ~(pinMask);

    // configure for interrupts to trigger on both edges (high and low)
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_IS_R_OFFSET)) &= ~(pinMask);
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_IBE_R_OFFSET)) |= pinMask;

    // Clear interrupt flags and re-enable
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_ICR_R_OFFSET)) |= pinMask;
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_IM_R_OFFSET)) |= pinMask;

    return;
}

void GPIO_ConfigInterrupts_LevelTrig(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask, bool highLevel) {
    // Disable interrupts
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_IM_R_OFFSET)) &= ~(pinMask);

    // configure for edge-triggered interrupts
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_IS_R_OFFSET)) |= pinMask;
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_IBE_R_OFFSET)) &= ~(pinMask);

    // select high or low level
    if(highLevel) {
        *((register_t) (gpioPort->BASE_ADDRESS + GPIO_IEV_R_OFFSET)) |= pinMask;
    }
    else {
        *((register_t) (gpioPort->BASE_ADDRESS + GPIO_IEV_R_OFFSET)) &= ~(pinMask);
    }

    // Clear interrupt flags and re-enable
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_ICR_R_OFFSET)) |= pinMask;
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_IM_R_OFFSET)) |= pinMask;

    return;
}

void GPIO_ConfigNVIC(GPIO_Port_t * gpioPort, uint8_t priority) {
    Assert(priority < 8);
    switch(gpioPort->BASE_ADDRESS) {
        case GPIO_PORTA_BASE_ADDRESS:
            NVIC_PRI0_R |= (priority << 5);
            NVIC_EN0_R |= (1 << 0);
            break;
        case GPIO_PORTB_BASE_ADDRESS:
            NVIC_PRI0_R |= (priority << 13);
            NVIC_EN0_R |= (1 << 1);
            break;
        case GPIO_PORTC_BASE_ADDRESS:
            NVIC_PRI0_R |= (priority << 21);
            NVIC_EN0_R |= (1 << 2);
            break;
        case GPIO_PORTD_BASE_ADDRESS:
            NVIC_PRI0_R |= (priority << 29);
            NVIC_EN0_R |= (1 << 3);
            break;
        case GPIO_PORTE_BASE_ADDRESS:
            NVIC_PRI0_R |= (priority << 5);
            NVIC_EN0_R |= (1 << 4);
            break;
        case GPIO_PORTF_BASE_ADDRESS:
            NVIC_PRI7_R |= (priority << 21);
            NVIC_EN0_R |= (1 << 30);
            break;
    }

    return;
}

/******************************************************************************
Basic Functions (Digital I/O)
*******************************************************************************/

uint8_t GPIO_ReadPins(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    return *((register_t) (gpioPort->BASE_ADDRESS + GPIO_DATA_R_OFFSET)) & pinMask;
}

void GPIO_WriteHigh(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_DATA_R_OFFSET)) |= pinMask;
    return;
}

void GPIO_WriteLow(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_DATA_R_OFFSET)) &= ~(pinMask);
    return;
}

void GPIO_Toggle(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_DATA_R_OFFSET)) ^= pinMask;
    return;
}

/******************************************************************************
Configuration (Alternate/Analog Modes)
*******************************************************************************/

void GPIO_ConfigAltMode(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_AFSEL_R_OFFSET)) |= pinMask;
    return;
}

void GPIO_ConfigPortCtrl(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask, uint8_t fieldEncoding) {
    // TODO: Write explanation
    register_t pctl_register = (register_t) (gpioPort->BASE_ADDRESS + GPIO_PCTL_R_OFFSET);
    for(uint8_t i = 0; i < 8; i++) {
        if(pinMask & (1 << i)) {
            *pctl_register |= (fieldEncoding << (4 * i));
        }
    }
}

void GPIO_ConfigAnalog(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    *((register_t) (gpioPort->BASE_ADDRESS + GPIO_AMSEL_R_OFFSET)) |= pinMask;
    return;
}
