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
struct GPIO_t {
    volatile uint32_t * DATA_R;
    volatile uint32_t * DIR_R;
    volatile uint32_t * INT_SENSE_R;
    volatile uint32_t * INT_BOTH_EDGES_R;
    volatile uint32_t * INT_EVENT_R;
    volatile uint32_t * INT_MASK_R;
    volatile uint32_t * INT_CLEAR_R;
    volatile uint32_t * ALT_FUNC_SELECT_R;
    volatile uint32_t * DRIVE_SELECT_2MA_R;
    volatile uint32_t * DRIVE_SELECT_4MA_R;
    volatile uint32_t * DRIVE_SELECT_8MA_R;
    volatile uint32_t * PULLUP_SELECT_R;
    volatile uint32_t * PULLDOWN_SELECT_R;
    volatile uint32_t * DIGITAL_ENABLE_R;
    volatile uint32_t * LOCK_R;
    volatile uint32_t * COMMIT_R;
    volatile uint32_t * ANALOG_MODE_SELECT_R;
    volatile uint32_t * PORT_CONTROL_R;
    bool isInit;
};

/******************************************************************************
GPIO Port Declarations
*******************************************************************************/
static GPIO_t PortA = { 0 };
static GPIO_t PortB = { 0 };
static GPIO_t PortC = { 0 };
static GPIO_t PortD = { 0 };
static GPIO_t PortE = { 0 };
static GPIO_t PortF = { 0 };

/******************************************************************************
Initialization
*******************************************************************************/
GPIO_t * GPIO_PortInit(uint8_t portName) {
    uint32_t base_address;
    GPIO_t * gpio_ptr;

    assert(portName < 5);

    // Determine the base address and struct pointer
    switch(portName) {
        case A:
            base_address = GPIO_PORTA_BASE_ADDRESS;
            gpio_ptr = &PortA;
            break;

        case B:
            base_address = GPIO_PORTB_BASE_ADDRESS;
            gpio_ptr = &PortB;
            break;

        case C:
            base_address = GPIO_PORTC_BASE_ADDRESS;
            gpio_ptr = &PortC;
            break;

        case D:
            base_address = GPIO_PORTD_BASE_ADDRESS;
            gpio_ptr = &PortD;
            break;

        case E:
            base_address = GPIO_PORTE_BASE_ADDRESS;
            gpio_ptr = &PortE;
            break;

        case F:
            base_address = GPIO_PORTF_BASE_ADDRESS;
            gpio_ptr = &PortF;
            break;

        default: assert(false);
    }

    if(gpio_ptr->isInit == false) {
        SYSCTL_RCGCGPIO_R |= (1 << portName);                            // start clock for port
        while((SYSCTL_PRGPIO_R & (1 << portName)) == 0) {}               // wait for it to be ready

        // Initialize struct members
        gpio_ptr->DATA_R = (volatile uint32_t *) (base_address + GPIO_DATA_R_OFFSET);
        gpio_ptr->DIR_R = (volatile uint32_t *) (base_address + GPIO_DIR_R_OFFSET);
        gpio_ptr->INT_SENSE_R = (volatile uint32_t *) (base_address + GPIO_IS_R_OFFSET);
        gpio_ptr->INT_BOTH_EDGES_R = (volatile uint32_t *) (base_address + GPIO_IBE_R_OFFSET);
        gpio_ptr->INT_EVENT_R = (volatile uint32_t *) (base_address + GPIO_IEV_R_OFFSET);
        gpio_ptr->INT_MASK_R = (volatile uint32_t *) (base_address + GPIO_IM_R_OFFSET);
        gpio_ptr->INT_CLEAR_R = (volatile uint32_t *) (base_address + GPIO_ICR_R_OFFSET);
        gpio_ptr->ALT_FUNC_SELECT_R = (volatile uint32_t *) (base_address + GPIO_AFSEL_R_OFFSET);
        gpio_ptr->DRIVE_SELECT_2MA_R = (volatile uint32_t *) (base_address + GPIO_DR2R_R_OFFSET);
        gpio_ptr->DRIVE_SELECT_4MA_R = (volatile uint32_t *) (base_address + GPIO_DR4R_R_OFFSET);
        gpio_ptr->DRIVE_SELECT_8MA_R = (volatile uint32_t *) (base_address + GPIO_DR8R_R_OFFSET);
        gpio_ptr->PULLUP_SELECT_R = (volatile uint32_t *) (base_address + GPIO_PUR_R_OFFSET);
        gpio_ptr->PULLDOWN_SELECT_R = (volatile uint32_t *) (base_address + GPIO_PDR_R_OFFSET);
        gpio_ptr->DIGITAL_ENABLE_R = (volatile uint32_t *) (base_address + GPIO_DEN_R_OFFSET);
        gpio_ptr->LOCK_R = (volatile uint32_t *) (base_address + GPIO_LOCK_R_OFFSET);
        gpio_ptr->COMMIT_R = (volatile uint32_t *) (base_address + GPIO_COMMIT_R_OFFSET);
        gpio_ptr->ANALOG_MODE_SELECT_R = (volatile uint32_t *) (base_address + GPIO_AMSEL_R_OFFSET);
        gpio_ptr->PORT_CONTROL_R = (volatile uint32_t *) (base_address + GPIO_PCTL_R_OFFSET);

        gpio_ptr->isInit = true;

        // Disable alternate and analog modes
        *(gpio_ptr->ANALOG_MODE_SELECT_R) &= ~(0xFF);
        *(gpio_ptr->ALT_FUNC_SELECT_R) &= ~(0xFF);
        *(gpio_ptr->PORT_CONTROL_R) = 0;
    }

    return gpio_ptr;
}

/******************************************************************************
Configuration (Digital I/O)
*******************************************************************************/

void GPIO_ConfigDir(GPIO_t * gpioPort, uint8_t bitMask) {
    *(gpioPort->DIR_R) |= bitMask;
}

void GPIO_ConfigPullUp(GPIO_t * gpioPort, uint8_t bitMask) {
    *(gpioPort->PULLUP_SELECT_R) |= bitMask;
}

void GPIO_ConfigPullDown(GPIO_t * gpioPort, uint8_t bitMask) {
    *(gpioPort->PULLDOWN_SELECT_R) |= bitMask;
}

void GPIO_ConfigDriveSelect(GPIO_t * gpioPort, uint8_t bitMask, uint8_t drive_mA) {
    switch(drive_mA) {
        case 2: *(gpioPort->DRIVE_SELECT_2MA_R) |= bitMask; break;
        case 4: *(gpioPort->DRIVE_SELECT_4MA_R) |= bitMask; break;
        case 8: *(gpioPort->DRIVE_SELECT_8MA_R) |= bitMask; break;
        default: assert(false);
    }
}

void GPIO_ConfigEnableDigital(GPIO_t * gpioPort, uint8_t bitMask) {
    *(gpioPort->DIGITAL_ENABLE_R) |= bitMask;
}

/******************************************************************************
Basic Functions (Digital I/O)
*******************************************************************************/

void GPIO_SetPins(GPIO_t * gpioPort, uint8_t bitMask) {

    *(gpioPort->DATA_R) |= bitMask;
}

void GPIO_ClearPins(GPIO_t * gpioPort, uint8_t bitMask) {

    *(gpioPort->DATA_R) &= ~(bitMask);
}
