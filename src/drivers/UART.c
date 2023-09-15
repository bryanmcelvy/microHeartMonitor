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
#include "FIFO.h"

#include "tm4c123gh6pm.h"
#include <stdbool.h>
#include <stdint.h>

#define ASCII_CONVERSION    0x30

#define UART0_TX_FULL       (UART0_FR_R & 0x20)

#define UART0_BUFFER_SIZE   16
#define UART0_INTERRUPT_NUM 5
/**********************************************************************
UART0
***********************************************************************/

static volatile uint32_t UART_buffer[UART0_BUFFER_SIZE] = { 0 };
static volatile FIFO_t * UART_fifo_ptr = 0;

void UART0_Init(void) {
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

    // activate clock for UART0 and wait for it to be ready
    SYSCTL_RCGCUART_R |= 0x01;
    while((SYSCTL_PRUART_R & 0x01) == 0) {}

    // initialize GPIO pins
    GPIO_Port_t * portA_ptr = GPIO_InitPort(A);
    GPIO_ConfigAltMode(portA_ptr, GPIO_PIN0 | GPIO_PIN1);
    GPIO_ConfigPortCtrl(portA_ptr, GPIO_PIN0 | GPIO_PIN1, 1);
    GPIO_ConfigDriveStrength(portA_ptr, GPIO_PIN0 | GPIO_PIN1, 8);
    GPIO_EnableDigital(portA_ptr, GPIO_PIN0 | GPIO_PIN1);

    // configure UART0
    UART0_CTL_R &= ~(0x01);                 // disable UART0
    UART0_IBRD_R |= 43;
    UART0_FBRD_R |= 26;
    UART0_LCRH_R |= 0x70;                   // 8-bit length, FIFO (NOTE: access *AFTER* `BRD`)
    UART0_CC_R &= ~(0x0F);                  // system clock source
    UART0_CTL_R |= 0x01;                    // re-enable UART0

    UART_fifo_ptr = FIFO_Init(UART_buffer, UART0_BUFFER_SIZE);

    NVIC_PRI1_R |= (1 << 13);               // priority 1
    NVIC_EN0_R |= (1 << UART0_INTERRUPT_NUM);               // enable UART0 interrupts in NVIC
}

unsigned char UART0_ReadChar(void) {
    /**
     * This function uses busy-wait synchronization
     * to read a character from UART0.
     */
    while((UART0_FR_R & 0x10) != 0) {}               // wait until Rx FIFO is empty
    return (unsigned char) (UART0_DR_R & 0xFF);
}

void UART0_WriteChar(unsigned char input_char) {
    /**
     * This function uses busy-wait synchronization
     * to write a character to UART0.
     */
    while((UART0_FR_R & 0x20) != 0) {}               // wait until Tx FIFO is no longer full
    UART0_DR_R = input_char;
}

void UART0_WriteStr(void * input_str) {
    /**
     * This function uses UART0_WriteChar() function
     * to write a C string to UART0. The function writes
     * until either the entire string has been written or
     * a null-terminated character has been reached.
     */
    unsigned char * str_ptr = input_str;
    while(*str_ptr != '\0') {
        UART0_WriteChar(*str_ptr);
        str_ptr += 1;
    }
}

void UART0_WriteInt(uint32_t n) {
    uint32_t nearestPowOf10 = 1;

    if(n < 10) {
        UART0_WriteChar(ASCII_CONVERSION + (n / nearestPowOf10));
    }
    else {
        while((n / (nearestPowOf10 * 10)) > 0) {
            nearestPowOf10 *= 10;
        }

        while(nearestPowOf10 > 0) {
            UART0_WriteChar(ASCII_CONVERSION + (n / nearestPowOf10));
            n %= nearestPowOf10;
            nearestPowOf10 /= 10;
        }
    }
}

void UART0_WriteFloat(double n, uint8_t num_decimals) {
    int32_t b;

    if(n < 0) {
        UART0_WriteChar('-');
        n *= -1;
    }

    b = n / (int32_t) 1;
    UART0_WriteInt(b);

    if(num_decimals > 0) {
        UART0_WriteChar('.');
        for(uint8_t count = 0; count < num_decimals; count++) {
            n = (n - b) * (double) 10;
            b = n / (int32_t) 1;
            UART0_WriteChar(ASCII_CONVERSION + b);
        }
    }
}

/**********************************************************************
UART0 (Interrupt)
***********************************************************************/

void UART0_IRQ_AddChar(unsigned char input_char) {
    FIFO_Put(UART_fifo_ptr, input_char);
    if(FIFO_isFull(UART_fifo_ptr)) {
        UART0_IRQ_Start();
    }
}

void UART0_IRQ_AddStr(void * input_str) {
    unsigned char * str_ptr = input_str;
    while(*str_ptr != '\0') {
        UART0_IRQ_AddChar(*str_ptr);
        str_ptr += 1;
    }
}

void UART0_IRQ_AddInt(uint32_t n) {
    uint32_t nearestPowOf10 = 1;

    if(n < 10) {
        UART0_IRQ_AddChar(ASCII_CONVERSION + (n / nearestPowOf10));
    }
    else {
        while((n / (nearestPowOf10 * 10)) > 0) {
            nearestPowOf10 *= 10;
        }

        while(nearestPowOf10 > 0) {
            UART0_IRQ_AddChar(ASCII_CONVERSION + (n / nearestPowOf10));
            n %= nearestPowOf10;
            nearestPowOf10 /= 10;
        }
    }
}

void UART0_IRQ_Start(void) {
    /**
     * This function writes to the Software Trigger Interrupt (`SWTRIG`)
     * register to activate the `UART0_Handler()` function rather than
     * relying on the TM4C123's built-in UART0 interrupt sources.
     */
    NVIC_SW_TRIG_R |= UART0_INTERRUPT_NUM;
}

void UART0_Handler(void) {
    while(FIFO_isEmpty(UART_fifo_ptr) == false) {
        unsigned char output_char = FIFO_Get(UART_fifo_ptr);
        while(UART0_TX_FULL) {}
        UART0_DR_R += output_char;
    }
}

/**********************************************************************
UART1
***********************************************************************/

void UART1_Init(void) {
    // clang-format off
    /**
     *  @details
     *  Given the bus frequency (`f_bus`) and desired baud rate (`BR`),
     *  the baud rate divisor (`BRD`) can be calculated:
     *      \f$BRD = f_{bus}/(16*BR)\f$
     *
     *  The integer BRD (`IBRD`) is simply the integer part of the BRD:
     *      \f$IBRD = int( BRD )\f$
     *
     *  The fractional BRD (`FBRD`) is calculated using the fractional part (`mod(BRD,1)`) of the BRD:
     *      \f$FBRD = int( ( mod(BRD,1) * 64 ) + 0.5 )\f$
     */
    // clang-format on

    SYSCTL_RCGCUART_R |= 0x02;                   // activate clock for UART1
    if((SYSCTL_RCGCGPIO_R & 0x02) == 0) {
        SYSCTL_RCGCGPIO_R |= 0x02;               // activate clock for GPIO Port B
    }

    UART1_CTL_R &= ~(0x01);                      // disable UART1
    UART1_IBRD_R |= 43;                          /// NOTE: LCRH must be accessed *AFTER* setting the
                                                 /// `BRD` register
    UART1_FBRD_R |= 26;
    UART1_LCRH_R |= 0x70;                        // 8-bit length, FIFO
    UART1_CC_R &= ~(0x0F);                       // system clock source
    UART1_CTL_R |= 0x01;                         // re-enable UART0

    GPIO_PORTB_AFSEL_R |= 0x03;                  // alt. mode for PB0/1
    GPIO_PORTB_PCTL_R |= 0x11;                   // UART mode for PB0/1
    GPIO_PORTB_DR8R_R |= 0x03;                   // 8 [ma] drive strength
    GPIO_PORTB_AMSEL_R &= ~(0x03);               // disable analog
    GPIO_PORTB_DEN_R |= 0x03;                    // enable digital I/O
}

unsigned char UART1_ReadChar(void) {
    /**
     * This function uses busy-wait synchronization
     * to read a character from UART1.
     */
    while((UART1_FR_R & 0x10) != 0) {}               // wait until Rx FIFO is empty
    return (unsigned char) (UART1_DR_R & 0xFF);
}

void UART1_WriteChar(unsigned char input_char) {
    /**
     * This function uses busy-wait synchronization
     * to write a character to UART1.
     */
    while((UART1_FR_R & 0x20) != 0) {}               // wait until Tx FIFO is no longer full
    UART1_DR_R = input_char;
}

void UART1_WriteStr(void * input_str) {
    /**
     * This function uses UART1_WriteChar() function
     * to write a C string to UART1. The function writes
     * until either the entire string has been written or
     * a null-terminated character has been reached.
     */
    unsigned char * str_ptr = input_str;
    while(*str_ptr != '\0') {
        UART1_WriteChar(*str_ptr);
        str_ptr += 1;
    }
}

/** @} */
