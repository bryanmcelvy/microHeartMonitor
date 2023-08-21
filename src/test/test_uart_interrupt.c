/**
 * @file
 * @author  Bryan McElvy
 * @brief   Test script for writing to serial port via UART0
 */

#include "PLL.h"
#include "GPIO.h"
#include "Timer.h"
#include "UART.h"
#include <stdint.h>

const uint8_t COLOR_LIST[8] = { 0,        LED_RED,  LED_YELLOW, LED_GREEN,
                                LED_CYAN, LED_BLUE, LED_PURPLE, LED_WHITE };
const char * COLOR_NAMES[8] = { "BLACK\n", "RED\n",  "YELLOW\n", "GREEN\n",
                                "CYAN\n",  "BLUE\n", "PURPLE\n", "WHITE\n" };

int main() {
    uint8_t idx;

    PLL_Init();
    Timer0A_Init();
    UART0_Init();
    GPIO_PF_LED_Init();

    UART0_IRQ_AddStr((unsigned char *) "Starting transmission...\r\n");
    UART0_IRQ_Start();

    idx = 0;
    while(1) {
        GPIO_PF_LED_Toggle(COLOR_LIST[idx]);
        UART0_IRQ_AddStr(COLOR_NAMES[idx]);
        UART0_IRQ_Start();
        Timer0A_Wait1ms(500);

        GPIO_PF_LED_Toggle(COLOR_LIST[idx]);
        UART0_IRQ_AddStr("OFF\n");
        UART0_IRQ_Start();
        Timer0A_Wait1ms(500);

        idx = (idx + 1) % 8;
    }
}
