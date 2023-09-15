/**
 * @addtogroup uart
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for UART module.
 */

#include "UART.h"

#include "GPIO.h"

#include "NewAssert.h"

#include "tm4c123gh6pm.h"
#include <stdbool.h>
#include <stdint.h>
#include <sys/_intsup.h>
#include <sys/_stdint.h>

#define ASCII_CONVERSION    0x30

#define UART0_TX_FULL       (UART0_FR_R & 0x20)

#define UART0_BUFFER_SIZE   16
#define UART0_INTERRUPT_NUM 5

enum {
    GPIO_PORTA_BASE_ADDRESS = (uint32_t) 0x40004000,
    GPIO_PORTB_BASE_ADDRESS = (uint32_t) 0x40005000,
    GPIO_PORTC_BASE_ADDRESS = (uint32_t) 0x40006000,
    GPIO_PORTD_BASE_ADDRESS = (uint32_t) 0x40007000,
    GPIO_PORTE_BASE_ADDRESS = (uint32_t) 0x40024000,
    GPIO_PORTF_BASE_ADDRESS = (uint32_t) 0x40025000
};

enum {
    UART0_BASE_ADDRESS = (uint32_t) 0x4000C000,
    UART1_BASE_ADDRESS = (uint32_t) 0x4000D000,
    UART2_BASE_ADDRESS = (uint32_t) 0x4000E000,
    UART3_BASE_ADDRESS = (uint32_t) 0x4000F000,
    UART4_BASE_ADDRESS = (uint32_t) 0x40010000,
    UART5_BASE_ADDRESS = (uint32_t) 0x40011000,
    UART6_BASE_ADDRESS = (uint32_t) 0x40012000,
    UART7_BASE_ADDRESS = (uint32_t) 0x40013000
};

enum {
    UART_DATA_R_OFFSET = (uint32_t) 0x00,
    UART_FR_R_OFFSET = (uint32_t) 0x18,
    CTL_R_OFFSET = (uint32_t) 0x30,
    IBRD_R_OFFSET = (uint32_t) 0x24,
    FBRD_R_OFFSET = (uint32_t) 0x28,
    LCRH_R_OFFSET = (uint32_t) 0x2C,
    CC_R_OFFSET = (uint32_t) 0xFC8
};

typedef volatile uint32_t * register_t;

/******************************************************************************
Initialization
*******************************************************************************/

struct UART_t {
    const uint32_t BASE_ADDRESS;
    const uint32_t FLAG_R_ADDRESS;
    GPIO_Port_t * GPIO_PORT;               ///< pointer to GPIO port data structure
    GPIO_Pin_t RX_PIN_NUM;                 ///< GPIO pin number
    GPIO_Pin_t TX_PIN_NUM;                 ///< GPIO pin number
    bool isInit;
};

static UART_t UART_ARR[8] = {
    { UART0_BASE_ADDRESS, UART0_BASE_ADDRESS + UART_FR_R_OFFSET, 0, GPIO_PIN0, GPIO_PIN1, false },
    { UART1_BASE_ADDRESS, UART1_BASE_ADDRESS + UART_FR_R_OFFSET, 0, GPIO_PIN0, GPIO_PIN1, false },
    { UART2_BASE_ADDRESS, UART2_BASE_ADDRESS + UART_FR_R_OFFSET, 0, GPIO_PIN6, GPIO_PIN7, false },
    { UART3_BASE_ADDRESS, UART3_BASE_ADDRESS + UART_FR_R_OFFSET, 0, GPIO_PIN6, GPIO_PIN7, false },
    { UART4_BASE_ADDRESS, UART4_BASE_ADDRESS + UART_FR_R_OFFSET, 0, GPIO_PIN4, GPIO_PIN5, false },
    { UART5_BASE_ADDRESS, UART5_BASE_ADDRESS + UART_FR_R_OFFSET, 0, GPIO_PIN4, GPIO_PIN5, false },
    { UART6_BASE_ADDRESS, UART6_BASE_ADDRESS + UART_FR_R_OFFSET, 0, GPIO_PIN4, GPIO_PIN5, false },
    { UART7_BASE_ADDRESS, UART7_BASE_ADDRESS + UART_FR_R_OFFSET, 0, GPIO_PIN0, GPIO_PIN1, false }
};

UART_t * UART_Init(GPIO_Port_t * port, UART_Num_t uartNum) {
    UART_t * uart = 0;
    uint32_t uartBaseAddr;

    // Check inputs
    Assert(GPIO_isPortInit(port));
    Assert(uartNum < 8);

    // Check that inputted GPIO port and UART match each other
    uint32_t gpio_baseAddress = GPIO_getBaseAddr(port);
    switch(uartNum) {
        case UART0: Assert(gpio_baseAddress == GPIO_PORTA_BASE_ADDRESS); break;
        case UART1: Assert(gpio_baseAddress == GPIO_PORTB_BASE_ADDRESS); break;
        case UART2:
        case UART6: Assert(gpio_baseAddress == GPIO_PORTD_BASE_ADDRESS); break;
        case UART3:
        case UART4: Assert(gpio_baseAddress == GPIO_PORTC_BASE_ADDRESS); break;
        case UART5:
        case UART7: Assert(gpio_baseAddress == GPIO_PORTE_BASE_ADDRESS); break;
    }
    uart = &UART_ARR[uartNum];
    uartBaseAddr = uart->BASE_ADDRESS;

    // Initialize UART
    if(uart->isInit == false) {
        SYSCTL_RCGCUART_R |= (1 << uartNum);
        while((SYSCTL_PRUART_R & (1 << uartNum)) == 0) {}

        // initialize GPIO pins
        GPIO_ConfigAltMode(port, uart->RX_PIN_NUM | uart->TX_PIN_NUM);
        if(gpio_baseAddress == GPIO_PORTC_BASE_ADDRESS) {
            GPIO_ConfigPortCtrl(port, uart->RX_PIN_NUM | uart->TX_PIN_NUM, 2);
        }
        else {
            GPIO_ConfigPortCtrl(port, uart->RX_PIN_NUM | uart->TX_PIN_NUM, 1);
        }
        GPIO_ConfigDriveStrength(port, uart->RX_PIN_NUM | uart->TX_PIN_NUM, 8);
        GPIO_EnableDigital(port, uart->RX_PIN_NUM | uart->TX_PIN_NUM);

        // disable UART
        *((register_t) (uartBaseAddr + CTL_R_OFFSET)) &= ~(1 << uartNum);

        // 8-bit length, FIFO
        *((register_t) (uartBaseAddr + IBRD_R_OFFSET)) |= 43;
        *((register_t) (uartBaseAddr + FBRD_R_OFFSET)) |= 26;

        // (NOTE: access *AFTER* `BRD`)
        *((register_t) (uartBaseAddr + LCRH_R_OFFSET)) |= 0x70;
        *((register_t) (uartBaseAddr + CC_R_OFFSET)) &= ~(0x0F);               // system clock

        // re-enable
        *((register_t) (uartBaseAddr + CTL_R_OFFSET)) |= (1 << uartNum);

        uart->isInit = true;
    }

    return uart;
}

/******************************************************************************
Reading
*******************************************************************************/

unsigned char UART_ReadChar(UART_t * uart) {
    while((uart->FLAG_R_ADDRESS & 0x10) != 0) {}
    return (unsigned char) *((register_t) (uart->BASE_ADDRESS));
}

/******************************************************************************
Writing
*******************************************************************************/

void UART_WriteChar(UART_t * uart, unsigned char input_char) {
    while((uart->FLAG_R_ADDRESS & 0x20) != 0) {}
    *((register_t) (uart->BASE_ADDRESS)) = input_char;
    return;
}

void UART_WriteStr(UART_t * uart, void * input_str) {
    unsigned char * str_ptr = input_str;
    while(*str_ptr != '\0') {
        UART_WriteChar(uart, *str_ptr);
        str_ptr += 1;
    }
    return;
}

void UART_WriteInt(UART_t * uart, uint32_t n) {
    uint32_t nearestPowOf10 = 1;

    if(n < 10) {
        UART_WriteChar(uart, ASCII_CONVERSION + (n / nearestPowOf10));
    }
    else {
        while((n / (nearestPowOf10 * 10)) > 0) {
            nearestPowOf10 *= 10;
        }

        while(nearestPowOf10 > 0) {
            UART_WriteChar(uart, ASCII_CONVERSION + (n / nearestPowOf10));
            n %= nearestPowOf10;
            nearestPowOf10 /= 10;
        }
    }
    return;
}

void UART_WriteFloat(UART_t * uart, double n, uint8_t num_decimals) {
    int32_t b;

    if(n < 0) {
        UART_WriteChar(uart, '-');
        n *= -1;
    }

    b = n / (int32_t) 1;
    UART_WriteInt(uart, b);

    if(num_decimals > 0) {
        UART_WriteChar(uart, '.');
        for(uint8_t count = 0; count < num_decimals; count++) {
            n = (n - b) * (double) 10;
            b = n / (int32_t) 1;
            UART_WriteChar(uart, ASCII_CONVERSION + b);
        }
    }
    return;
}

/** @} */
