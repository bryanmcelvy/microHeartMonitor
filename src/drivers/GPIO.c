/**
 * @addtogroup gpio
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for GPIO module.
 */

#include "GPIO.h"

/******************************************************************************
Preprocessor Directives
*******************************************************************************/

#include "sysctrl.h"

#include "NewAssert.h"

#include "m-profile/cmsis_gcc_m.h"
#include "tm4c123gh6pm.h"

#include <stdbool.h>
#include <stdint.h>

#define GPIO_NUM_PORTS 6

// Base Addresses
enum GPIO_PORT_BASE_ADDRESSES {
    GPIO_PORTA_BASE_ADDRESS = (uint32_t) 0x40004000,
    GPIO_PORTB_BASE_ADDRESS = (uint32_t) 0x40005000,
    GPIO_PORTC_BASE_ADDRESS = (uint32_t) 0x40006000,
    GPIO_PORTD_BASE_ADDRESS = (uint32_t) 0x40007000,
    GPIO_PORTE_BASE_ADDRESS = (uint32_t) 0x40024000,
    GPIO_PORTF_BASE_ADDRESS = (uint32_t) 0x40025000,
};

// Register Offsets
enum GPIO_REGISTER_OFFSETS {
    DATA_REG_OFFSET = (uint32_t) 0x03FC,                         ///< data
    DIRECTION_REG_OFFSET = (uint32_t) 0x0400,                    ///< direction
    INT_SENSE_REG_OFFSET = (uint32_t) 0x0404,                    ///< interrupt sense
    INT_BOTH_EDGE_REG_OFFSET = (uint32_t) 0x0408,                ///< interrupt both edges
    INT_EVENT_REG_OFFSET = (uint32_t) 0x040C,                    ///< interrupt event
    INT_MASK_REG_OFFSET = (uint32_t) 0x0410,                     ///< interrupt mask
    INT_CLEAR_REG_OFFSET = (uint32_t) 0x041C,                    ///< interrupt clear
    ALT_FUNC_REG_OFFSET = (uint32_t) 0x0420,                     ///< alternate function select
    DRIVE_STR_2MA_REG_OFFSET = (uint32_t) 0x0500,                ///< drive strength (2 [ma])
    DRIVE_STR_4MA_REG_OFFSET = (uint32_t) 0x0504,                ///< drive strength (4 [ma])
    DRIVE_STR_8MA_REG_OFFSET = (uint32_t) 0x0508,                ///< drive strength (8 [ma])
    PULLUP_REG_OFFSET = (uint32_t) 0x0510,                       ///< pull-up resistor
    PULLDOWN_REG_OFFSET = (uint32_t) 0x0518,                     ///< pull-down resistor
    DIGITAL_ENABLE_REG_OFFSET = (uint32_t) 0x051C,               ///< digital enable
    LOCK_REG_OFFSET = (uint32_t) 0x0520,                         ///< lock
    COMMIT_REG_OFFSET = (uint32_t) 0x0524,                       ///< commit
    ALT_MODE_REG_OFFSET = (uint32_t) 0x0528,                     ///< alternate mode select
    PORT_CTRL_REG_OFFSET = (uint32_t) 0x052C                     ///< port control
};

/******************************************************************************
Struct Definition
*******************************************************************************/

typedef struct GpioPortStruct_t {
    uint32_t BASE_ADDRESS;
    uint32_t DATA_REGISTER;
    bool * isInit;
} GpioPortStruct_t;

static bool initStatusArray[6] = { false, false, false, false, false, false };

// clang-format off
static const GpioPortStruct_t GPIO_STRUCT_ARRAY[6] = {
    { GPIO_PORTA_BASE_ADDRESS, (GPIO_PORTA_BASE_ADDRESS + DATA_REG_OFFSET), &initStatusArray[0] },
    { GPIO_PORTB_BASE_ADDRESS, (GPIO_PORTB_BASE_ADDRESS + DATA_REG_OFFSET), &initStatusArray[1] },
    { GPIO_PORTC_BASE_ADDRESS, (GPIO_PORTC_BASE_ADDRESS + DATA_REG_OFFSET), &initStatusArray[2] },
    { GPIO_PORTD_BASE_ADDRESS, (GPIO_PORTD_BASE_ADDRESS + DATA_REG_OFFSET), &initStatusArray[3] },
    { GPIO_PORTE_BASE_ADDRESS, (GPIO_PORTE_BASE_ADDRESS + DATA_REG_OFFSET), &initStatusArray[4] },
    { GPIO_PORTF_BASE_ADDRESS, (GPIO_PORTF_BASE_ADDRESS + DATA_REG_OFFSET), &initStatusArray[5] }
};               // clang-format on

/******************************************************************************
Initialization
*******************************************************************************/

GpioPort_t GPIO_InitPort(GPIO_PortName_t portName) {
    assert(portName < GPIO_NUM_PORTS);

    GpioPort_t gpioPort = &GPIO_STRUCT_ARRAY[portName];
    if(*gpioPort->isInit == false) {
        SysCtrl_configPeripheralClk(SYSCTRL_RUN, SYSCTRL_GPIO, portName, SYSCTRL_CLK_ON);

        // Disable alternate and analog modes
        REGISTER_VAL(gpioPort->BASE_ADDRESS + ALT_MODE_REG_OFFSET) &= ~(0xFF);
        REGISTER_VAL(gpioPort->BASE_ADDRESS + ALT_FUNC_REG_OFFSET) &= ~(0xFF);
        REGISTER_VAL(gpioPort->BASE_ADDRESS + PORT_CTRL_REG_OFFSET) = 0;

        if(portName == F) {
            GPIO_PORTF_LOCK_R = 0x4C4F434B;               // Unlock GPIO Port F
            GPIO_PORTF_CR_R |= 0x01;                      // Allow changes to PF0
        }

        *gpioPort->isInit = true;
    }

    return gpioPort;
}

bool GPIO_isPortInit(GpioPort_t gpioPort) {
    return *gpioPort->isInit;
}

uint32_t GPIO_getBaseAddr(GpioPort_t gpioPort) {
    assert(*gpioPort->isInit);
    return gpioPort->BASE_ADDRESS;
}

/******************************************************************************
Configuration (Digital I/O)
*******************************************************************************/

void GPIO_configDirection(GpioPort_t gpioPort, GpioPin_t pinMask, gpioDir_t direction) {
    assert(*gpioPort->isInit);

    switch(direction) {
        case GPIO_INPUT:
            REGISTER_VAL(gpioPort->BASE_ADDRESS + DIRECTION_REG_OFFSET) &= ~(pinMask);
            break;
        case GPIO_OUTPUT:
            REGISTER_VAL(gpioPort->BASE_ADDRESS + DIRECTION_REG_OFFSET) |= pinMask;
            break;
        default:
            assert(false);
    }

    return;
}

void GPIO_configResistor(GpioPort_t gpioPort, GpioPin_t pinMask, gpioResistor_t resistor) {
    assert(*gpioPort->isInit);

    uint32_t registerOffset;
    switch(resistor) {
        case PULLUP:
            registerOffset = PULLUP_REG_OFFSET;
            break;
        case PULLDOWN:
            registerOffset = PULLDOWN_REG_OFFSET;
            break;
        default:
            assert(false);
    }

    REGISTER_VAL(gpioPort->BASE_ADDRESS + registerOffset) |= pinMask;
    return;
}

void GPIO_ConfigDriveStrength(GpioPort_t gpioPort, GpioPin_t pinMask, uint8_t drive_mA) {
    assert(*gpioPort->isInit);

    uint32_t driveSelectRegister_Offset;
    switch(drive_mA) {
        case 2:
            driveSelectRegister_Offset = DRIVE_STR_2MA_REG_OFFSET;
            break;
        case 4:
            driveSelectRegister_Offset = DRIVE_STR_4MA_REG_OFFSET;
            break;
        case 8:
            driveSelectRegister_Offset = DRIVE_STR_8MA_REG_OFFSET;
            break;
        default:
            driveSelectRegister_Offset = 0;
            assert(false);
    }
    REGISTER_VAL(gpioPort->BASE_ADDRESS + driveSelectRegister_Offset) |= pinMask;
    return;
}

void GPIO_EnableDigital(GpioPort_t gpioPort, GpioPin_t pinMask) {
    assert(*gpioPort->isInit);

    REGISTER_VAL(gpioPort->BASE_ADDRESS + DIGITAL_ENABLE_REG_OFFSET) |= pinMask;
    return;
}

void GPIO_DisableDigital(GpioPort_t gpioPort, GpioPin_t pinMask) {
    assert(*gpioPort->isInit);

    REGISTER_VAL(gpioPort->BASE_ADDRESS + DIGITAL_ENABLE_REG_OFFSET) &= ~pinMask;
    return;
}

/******************************************************************************
Configuration (Interrupts)
*******************************************************************************/

void GPIO_ConfigInterrupts_Edge(GpioPort_t gpioPort, GpioPin_t pinMask, bool risingEdge) {
    assert(*gpioPort->isInit);

    // Disable interrupts
    REGISTER_VAL(gpioPort->BASE_ADDRESS + INT_MASK_REG_OFFSET) &= ~(pinMask);

    // configure for edge-triggered interrupts
    REGISTER_VAL(gpioPort->BASE_ADDRESS + INT_SENSE_REG_OFFSET) &= ~(pinMask);
    REGISTER_VAL(gpioPort->BASE_ADDRESS + INT_BOTH_EDGE_REG_OFFSET) &= ~(pinMask);

    // select high or low edge
    if(risingEdge) {
        REGISTER_VAL(gpioPort->BASE_ADDRESS + INT_EVENT_REG_OFFSET) |= pinMask;
    }
    else {
        REGISTER_VAL(gpioPort->BASE_ADDRESS + INT_EVENT_REG_OFFSET) &= ~(pinMask);
    }

    // Clear interrupt flags and re-enable
    REGISTER_VAL(gpioPort->BASE_ADDRESS + INT_CLEAR_REG_OFFSET) |= pinMask;
    REGISTER_VAL(gpioPort->BASE_ADDRESS + INT_MASK_REG_OFFSET) |= pinMask;

    return;
}

void GPIO_ConfigInterrupts_BothEdges(GpioPort_t gpioPort, GpioPin_t pinMask) {
    assert(*gpioPort->isInit);

    // Disable interrupts
    REGISTER_VAL(gpioPort->BASE_ADDRESS + INT_MASK_REG_OFFSET) &= ~(pinMask);

    // configure for interrupts to trigger on both edges (high and low)
    REGISTER_VAL(gpioPort->BASE_ADDRESS + INT_SENSE_REG_OFFSET) &= ~(pinMask);
    REGISTER_VAL(gpioPort->BASE_ADDRESS + INT_BOTH_EDGE_REG_OFFSET) |= pinMask;

    // Clear interrupt flags and re-enable
    REGISTER_VAL(gpioPort->BASE_ADDRESS + INT_CLEAR_REG_OFFSET) |= pinMask;
    REGISTER_VAL(gpioPort->BASE_ADDRESS + INT_MASK_REG_OFFSET) |= pinMask;

    return;
}

void GPIO_ConfigInterrupts_LevelTrig(GpioPort_t gpioPort, GpioPin_t pinMask, bool highLevel) {
    assert(*gpioPort->isInit);

    // Disable interrupts
    REGISTER_VAL(gpioPort->BASE_ADDRESS + INT_MASK_REG_OFFSET) &= ~(pinMask);

    // configure for edge-triggered interrupts
    REGISTER_VAL(gpioPort->BASE_ADDRESS + INT_SENSE_REG_OFFSET) |= pinMask;
    REGISTER_VAL(gpioPort->BASE_ADDRESS + INT_BOTH_EDGE_REG_OFFSET) &= ~(pinMask);

    // select high or low level
    if(highLevel) {
        REGISTER_VAL(gpioPort->BASE_ADDRESS + INT_EVENT_REG_OFFSET) |= pinMask;
    }
    else {
        REGISTER_VAL(gpioPort->BASE_ADDRESS + INT_EVENT_REG_OFFSET) &= ~(pinMask);
    }

    // Clear interrupt flags and re-enable
    REGISTER_VAL(gpioPort->BASE_ADDRESS + INT_CLEAR_REG_OFFSET) |= pinMask;
    REGISTER_VAL(gpioPort->BASE_ADDRESS + INT_MASK_REG_OFFSET) |= pinMask;

    return;
}

void GPIO_ConfigNVIC(GpioPort_t gpioPort, uint8_t priority) {
    assert(*gpioPort->isInit);
    assert(priority < 8);

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
            NVIC_PRI1_R |= (priority << 5);
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

volatile uint32_t * GPIO_getDataRegister(GpioPort_t gpioPort) {
    assert(*gpioPort->isInit);
    return ((volatile uint32_t *) gpioPort->DATA_REGISTER);
}

uint8_t GPIO_ReadPins(GpioPort_t gpioPort, GpioPin_t pinMask) {
    assert(*gpioPort->isInit);
    return REGISTER_VAL(gpioPort->DATA_REGISTER) & pinMask;
}

void GPIO_WriteHigh(GpioPort_t gpioPort, GpioPin_t pinMask) {
    assert(*gpioPort->isInit);
    REGISTER_VAL(gpioPort->DATA_REGISTER) |= pinMask;
    return;
}

void GPIO_WriteLow(GpioPort_t gpioPort, GpioPin_t pinMask) {
    assert(*gpioPort->isInit);
    REGISTER_VAL(gpioPort->DATA_REGISTER) &= ~(pinMask);
    return;
}

void GPIO_Toggle(GpioPort_t gpioPort, GpioPin_t pinMask) {
    assert(*gpioPort->isInit);
    REGISTER_VAL(gpioPort->DATA_REGISTER) ^= pinMask;
    return;
}

/******************************************************************************
Configuration (Alternate/Analog Modes)
*******************************************************************************/

void GPIO_ConfigAltMode(GpioPort_t gpioPort, GpioPin_t pinMask) {
    assert(*gpioPort->isInit);
    REGISTER_VAL(gpioPort->BASE_ADDRESS + ALT_FUNC_REG_OFFSET) |= pinMask;
    return;
}

void GPIO_ConfigPortCtrl(GpioPort_t gpioPort, GpioPin_t pinMask, uint8_t fieldEncoding) {
    assert(*gpioPort->isInit);

    // TODO: Write explanation
    register_t portCtrlRegister = REGISTER_CAST(gpioPort->BASE_ADDRESS + PORT_CTRL_REG_OFFSET);
    for(uint8_t i = 0; i < 8; i++) {
        if(pinMask & (1 << i)) {
            *portCtrlRegister |= (fieldEncoding << (4 * i));
        }
    }
    return;
}

void GPIO_ConfigAnalog(GpioPort_t gpioPort, GpioPin_t pinMask) {
    assert(*gpioPort->isInit);
    REGISTER_VAL(gpioPort->BASE_ADDRESS + ALT_MODE_REG_OFFSET) |= pinMask;
    return;
}

/** @} */               // gpio
