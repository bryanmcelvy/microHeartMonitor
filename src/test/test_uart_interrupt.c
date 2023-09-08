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

int main(void) {
    uint8_t idx;

    PLL_Init();
    Timer0A_Init();
    UART0_Init();

    // Init. LED pins
    GPIO_Port_t * portF = GPIO_InitPort(F);
    GPIO_ConfigDirOutput(portF, (GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3));
    GPIO_ConfigDriveStrength(portF, (GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3), 8);
    GPIO_EnableDigital(portF, (GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3));

    UART0_IRQ_AddStr((unsigned char *) "Starting transmission...\r\n");
    UART0_IRQ_Start();

    idx = 0;
    while(1) {
        GPIO_Toggle(portF, COLOR_LIST[idx]);
        UART0_IRQ_AddStr(COLOR_NAMES[idx]);
        UART0_IRQ_Start();
        Timer0A_Wait1ms(500);

        GPIO_Toggle(portF, COLOR_LIST[idx]);
        UART0_IRQ_AddStr("OFF\n");
        UART0_IRQ_Start();
        Timer0A_Wait1ms(500);

        idx = (idx + 1) % 8;
    }
}
