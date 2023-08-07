/**
 * @addtogroup uart
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for UART module.
 */

#include "UART.h"

#include "tm4c123gh6pm.h"

/**********************************************************************
UART0
***********************************************************************/

void UART0_Init(void) {
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
    
    SYSCTL_RCGCUART_R |= 0x01;                  // activate clock for UART0
    if ((SYSCTL_RCGCGPIO_R & 0x01) == 0) {
        SYSCTL_RCGCGPIO_R |= 0x01;              // activate clock for GPIO Port A
    }

    UART0_CTL_R &= ~(0x01);                     // disable UART0
    UART0_IBRD_R |= 43;                         /// NOTE: LCRH must be accessed *AFTER* setting the `BRD` register0
    UART0_FBRD_R |= 26;
    UART0_LCRH_R |= 0x70;                       // 8-bit length, FIFO
    UART0_CC_R &= ~(0x0F);                      // system clock source
    UART0_CTL_R |= 0x01;                        // re-enable UART0

    GPIO_PORTA_AFSEL_R |= 0x03;                 // alt. mode for PA0/1
    GPIO_PORTA_PCTL_R |= 0x11;                  // UART mode for PA0/1
    GPIO_PORTA_DR8R_R |= 0x03;                  // 8 [ma] drive strength
    GPIO_PORTA_AMSEL_R &= ~(0x03);              // disable analog
    GPIO_PORTA_DEN_R |= 0x03;                   // enable digital I/O
}

unsigned char UART0_ReadChar(void) {
    /**
     * This function uses busy-wait synchronization 
     * to read a character from UART0.
     */
    while( (UART0_FR_R & 0x10) != 0) {}         // wait until Rx FIFO is empty
    return (unsigned char) (UART0_DR_R & 0xFF);
}

void UART0_WriteChar(unsigned char input_char) {
    /**
     * This function uses busy-wait synchronization 
     * to write a character to UART0.
     */
    while( (UART0_FR_R & 0x20) != 0) {}         // wait until Tx FIFO is no longer full
    UART0_DR_R = input_char;
}

void UART0_WriteStr(unsigned char * str_ptr) {
    /**
     * This function uses UART0_WriteChar() function 
     * to write a C string to UART0. The function writes 
     * until either the entire string has been written or 
     * a null-terminated character has been reached.
     */
    while (*str_ptr != '\0') {
        UART0_WriteChar(*str_ptr);
        str_ptr += 1;
    }
}

void UART0_WriteInt(uint32_t n) {
    uint32_t nearestPowOf10 = 1;

    while( (n / (nearestPowOf10 * 10) ) > 0) {
        nearestPowOf10 *= 10;
    }

    while(nearestPowOf10 > 0) {
        UART0_WriteChar( 0x30 + (n / nearestPowOf10) );
        n %= nearestPowOf10;
        nearestPowOf10 /= 10;
    }
}

/**********************************************************************
UART1
***********************************************************************/

void UART1_Init(void) {
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
    
    SYSCTL_RCGCUART_R |= 0x02;                  // activate clock for UART1
    if ((SYSCTL_RCGCGPIO_R & 0x02) == 0) {
        SYSCTL_RCGCGPIO_R |= 0x02;              // activate clock for GPIO Port B
    }

    UART1_CTL_R &= ~(0x01);                     // disable UART1
    UART1_IBRD_R |= 43;                         /// NOTE: LCRH must be accessed *AFTER* setting the `BRD` register
    UART1_FBRD_R |= 26;
    UART1_LCRH_R |= 0x70;                       // 8-bit length, FIFO
    UART1_CC_R &= ~(0x0F);                      // system clock source
    UART1_CTL_R |= 0x01;                        // re-enable UART0

    GPIO_PORTB_AFSEL_R |= 0x03;                 // alt. mode for PB0/1
    GPIO_PORTB_PCTL_R |= 0x11;                  // UART mode for PB0/1
    GPIO_PORTB_DR8R_R |= 0x03;                  // 8 [ma] drive strength
    GPIO_PORTB_AMSEL_R &= ~(0x03);              // disable analog
    GPIO_PORTB_DEN_R |= 0x03;                   // enable digital I/O
}

unsigned char UART1_ReadChar(void) {
    /**
     * This function uses busy-wait synchronization 
     * to read a character from UART1.
     */
    while( (UART1_FR_R & 0x10) != 0) {}         // wait until Rx FIFO is empty
    return (unsigned char) (UART1_DR_R & 0xFF);
}

void UART1_WriteChar(unsigned char input_char) {
    /**
     * This function uses busy-wait synchronization 
     * to write a character to UART1.
     */
    while( (UART1_FR_R & 0x20) != 0) {}         // wait until Tx FIFO is no longer full
    UART1_DR_R = input_char;
}

void UART1_WriteStr(unsigned char * str_ptr) {
    /**
     * This function uses UART1_WriteChar() function 
     * to write a C string to UART0. The function writes 
     * until either the entire string has been written or 
     * a null-terminated character has been reached.
     */
    while (*str_ptr != '\0') {
        UART1_WriteChar(*str_ptr);
        str_ptr += 1;
    }
}

/** @} */
