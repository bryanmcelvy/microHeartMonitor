/**
 * @addtogroup uart
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for UART module.
 */

#include "UART.h"

/******************************************************************************
SECTIONS
        Constant Declarations
        Initialization
        Reading
        Writing
*******************************************************************************/

#include "GPIO.h"

#include "NewAssert.h"

#include "m-profile/cmsis_gcc_m.h"
#include "tm4c123gh6pm.h"

#include <stdbool.h>
#include <stdint.h>

#define CONVERT_INT_TO_ASCII(X) ((unsigned char) (X + 0x30))

/******************************************************************************
Constant Declarations
*******************************************************************************/

enum GPIO_BASE_ADDRESSES {
    GPIO_PORTA_BASE = (uint32_t) 0x40004000,
    GPIO_PORTB_BASE = (uint32_t) 0x40005000,
    GPIO_PORTC_BASE = (uint32_t) 0x40006000,
    GPIO_PORTD_BASE = (uint32_t) 0x40007000,
    GPIO_PORTE_BASE = (uint32_t) 0x40024000,
    GPIO_PORTF_BASE = (uint32_t) 0x40025000
};

enum UART_BASE_ADDRESSES {
    UART0_BASE = (uint32_t) 0x4000C000,
    UART1_BASE = (uint32_t) 0x4000D000,
    UART2_BASE = (uint32_t) 0x4000E000,
    UART3_BASE = (uint32_t) 0x4000F000,
    UART4_BASE = (uint32_t) 0x40010000,
    UART5_BASE = (uint32_t) 0x40011000,
    UART6_BASE = (uint32_t) 0x40012000,
    UART7_BASE = (uint32_t) 0x40013000
};

enum UART_REG_OFFSETS {
    UART_FR_R_OFFSET = (uint32_t) 0x18,
    IBRD_R_OFFSET = (uint32_t) 0x24,
    FBRD_R_OFFSET = (uint32_t) 0x28,
    LCRH_R_OFFSET = (uint32_t) 0x2C,
    CTL_R_OFFSET = (uint32_t) 0x30,
    CC_R_OFFSET = (uint32_t) 0xFC8
};

/******************************************************************************
Initialization
*******************************************************************************/

typedef struct UartStruct_t {
    uint32_t BASE_ADDRESS;
    register_t FLAG_REGISTER;
    bool * isInitPtr;
} UartStruct_t;

static bool initStatusArray[8] = { false, false, false, false, false, false, false, false };

// clang-format off
static const UartStruct_t UART_STRUCT_ARRAY[8] = {
    { UART0_BASE, REGISTER_CAST(UART0_BASE + UART_FR_R_OFFSET), &initStatusArray[0] },
    { UART1_BASE, REGISTER_CAST(UART1_BASE + UART_FR_R_OFFSET), &initStatusArray[1] },
    { UART2_BASE, REGISTER_CAST(UART2_BASE + UART_FR_R_OFFSET), &initStatusArray[2] },
    { UART3_BASE, REGISTER_CAST(UART3_BASE + UART_FR_R_OFFSET), &initStatusArray[3] },
    { UART4_BASE, REGISTER_CAST(UART4_BASE + UART_FR_R_OFFSET), &initStatusArray[4] },
    { UART5_BASE, REGISTER_CAST(UART5_BASE + UART_FR_R_OFFSET), &initStatusArray[5] },
    { UART6_BASE, REGISTER_CAST(UART6_BASE + UART_FR_R_OFFSET), &initStatusArray[6] },
    { UART7_BASE, REGISTER_CAST(UART7_BASE + UART_FR_R_OFFSET), &initStatusArray[7] }
};               // clang-format on

Uart_t UART_Init(GpioPort_t port, uartNum_t uartNum) {
    // Check inputs
    assert(GPIO_isPortInit(port));
    assert(uartNum < 8);

    // Check that inputted GPIO port and UART match each other
    uint32_t gpio_baseAddress = GPIO_getBaseAddr(port);
    GpioPin_t RX_PIN_NUM;
    GpioPin_t TX_PIN_NUM;

    switch(uartNum) {
        case UART0:
            assert(gpio_baseAddress == GPIO_PORTA_BASE);
            RX_PIN_NUM = GPIO_PIN0;
            TX_PIN_NUM = GPIO_PIN1;
            break;
        case UART1:
            assert(gpio_baseAddress == GPIO_PORTB_BASE);
            RX_PIN_NUM = GPIO_PIN0;
            TX_PIN_NUM = GPIO_PIN1;
            break;
        case UART2:
            assert(gpio_baseAddress == GPIO_PORTD_BASE);
            RX_PIN_NUM = GPIO_PIN6;
            TX_PIN_NUM = GPIO_PIN7;
            break;
        case UART3:
            assert(gpio_baseAddress == GPIO_PORTC_BASE);
            RX_PIN_NUM = GPIO_PIN6;
            TX_PIN_NUM = GPIO_PIN7;
            break;
        case UART4:
            assert(gpio_baseAddress == GPIO_PORTC_BASE);
            RX_PIN_NUM = GPIO_PIN4;
            TX_PIN_NUM = GPIO_PIN5;
            break;
        case UART5:
            assert(gpio_baseAddress == GPIO_PORTE_BASE);
            RX_PIN_NUM = GPIO_PIN4;
            TX_PIN_NUM = GPIO_PIN5;
            break;
        case UART6:
            assert(gpio_baseAddress == GPIO_PORTD_BASE);
            RX_PIN_NUM = GPIO_PIN4;
            TX_PIN_NUM = GPIO_PIN5;
            break;
        case UART7:
            assert(gpio_baseAddress == GPIO_PORTE_BASE);
            RX_PIN_NUM = GPIO_PIN0;
            TX_PIN_NUM = GPIO_PIN1;
            break;
    }

    // clang-format off
    /**
     *  @details
     *  Given the bus frequency (`f_bus`) and desired baud rate (`BR`),
     *  the baud rate divisor (`BRD`) can be calculated:
     *      \f$ BRD = f_{bus}/(16*BR) \f$
     * 
     *  The integer BRD (`IBRD`) is simply the integer part of the BRD:
     *      \f$ IBRD = int( BRD ) \f$
     * 
     *  The fractional BRD (`FBRD`) is calculated using the fractional part (`mod(BRD,1)`) of the BRD:
     *      \f$ FBRD = int( ( mod(BRD,1) * 64 ) + 0.5 ) \f$
     */
    // clang-format on

    // Initialize UART
    Uart_t uart = &UART_STRUCT_ARRAY[uartNum];
    if(*uart->isInitPtr == false) {
        SYSCTL_RCGCUART_R |= (1 << uartNum);
        while((SYSCTL_PRUART_R & (1 << uartNum)) == 0) {
            __NOP();
        }

        // initialize GPIO pins
        GPIO_ConfigAltMode(port, RX_PIN_NUM | TX_PIN_NUM);
        if(gpio_baseAddress == GPIO_PORTC_BASE) {
            GPIO_ConfigPortCtrl(port, RX_PIN_NUM | TX_PIN_NUM, 2);
        }
        else {
            GPIO_ConfigPortCtrl(port, RX_PIN_NUM | TX_PIN_NUM, 1);
        }
        GPIO_ConfigDriveStrength(port, RX_PIN_NUM | TX_PIN_NUM, 8);
        GPIO_EnableDigital(port, RX_PIN_NUM | TX_PIN_NUM);

        // disable UART
        REGISTER_VAL(uart->BASE_ADDRESS + CTL_R_OFFSET) &= ~(1 << uartNum);

        // 8-bit length, FIFO
        REGISTER_VAL(uart->BASE_ADDRESS + IBRD_R_OFFSET) |= 43;
        REGISTER_VAL(uart->BASE_ADDRESS + FBRD_R_OFFSET) |= 26;

        // (NOTE: access *AFTER* `BRD`)
        REGISTER_VAL(uart->BASE_ADDRESS + LCRH_R_OFFSET) |= 0x70;
        REGISTER_VAL(uart->BASE_ADDRESS + CC_R_OFFSET) &= ~(0x0F);               // system clock

        // re-enable
        REGISTER_VAL(uart->BASE_ADDRESS + CTL_R_OFFSET) |= (1 << uartNum);

        *uart->isInitPtr = true;
    }

    return uart;
}

bool UART_isInit(Uart_t uart) {
    return *uart->isInitPtr;
}

/******************************************************************************
Reading
*******************************************************************************/

unsigned char UART_ReadChar(Uart_t uart) {
    while((*uart->FLAG_REGISTER & 0x10) != 0) {
        __NOP();
    }
    return (unsigned char) REGISTER_VAL(uart->BASE_ADDRESS);
}

/******************************************************************************
Writing
*******************************************************************************/

void UART_WriteChar(Uart_t uart, unsigned char inputChar) {
    while((*uart->FLAG_REGISTER & 0x20) != 0) {
        __NOP();
    }
    REGISTER_VAL(uart->BASE_ADDRESS) = inputChar;
    return;
}

void UART_WriteStr(Uart_t uart, void * inputStr) {
    unsigned char * str_ptr = inputStr;
    while(*str_ptr != '\0') {
        UART_WriteChar(uart, *str_ptr);
        str_ptr += 1;
    }
    return;
}

void UART_WriteInt(Uart_t uart, int32_t n) {
    // Send negative sign (`-`) if needed
    if(n < 0) {
        UART_WriteChar(uart, '-');
        n *= -1;
    }

    if(n < 10) {
        UART_WriteChar(uart, CONVERT_INT_TO_ASCII(n));
    }
    else {
        int32_t nearestPowOf10 = 1;
        while((n / (nearestPowOf10 * 10)) > 0) {
            nearestPowOf10 *= 10;
        }

        while(nearestPowOf10 > 0) {
            UART_WriteChar(uart, CONVERT_INT_TO_ASCII(n / nearestPowOf10));
            n %= nearestPowOf10;
            nearestPowOf10 /= 10;
        }
    }
    return;
}

void UART_WriteFloat(Uart_t uart, float n, uint8_t numDecimals) {
    // Send negative sign (`-`) if needed
    if(n < 0) {
        UART_WriteChar(uart, '-');
        n *= -1;
    }

    // Send the integer part
    int32_t b = n / (int32_t) 1;
    UART_WriteInt(uart, b);

    // Send the decimal part
    if(numDecimals > 0) {
        UART_WriteChar(uart, '.');
        for(uint8_t count = 0; count < numDecimals; count++) {
            n = (n - b) * (double) 10;
            b = n / (int32_t) 1;
            UART_WriteChar(uart, CONVERT_INT_TO_ASCII(b));
        }
    }
    return;
}

/** @} */
