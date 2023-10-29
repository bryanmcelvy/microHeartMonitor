/**
 * @file
 * @author  Bryan McElvy
 * @brief   Test script for Debug module.
 */

#include "Debug.h"

#include "GPIO.h"
#include "PLL.h"
#include "Timer.h"

#include <stdint.h>

int main(void) {
    PLL_Init();
    Timer_t timer0 = Timer_Init(TIMER0);
    Timer_setMode(timer0, ONESHOT, UP);

    // Init. LED pins
    GpioPort_t portF = GPIO_InitPort(F);
    GPIO_configDirection(portF, (GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3), GPIO_OUTPUT);
    GPIO_ConfigDriveStrength(portF, (GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3), 8);
    GPIO_EnableDigital(portF, (GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3));

    GpioPort_t portA = GPIO_InitPort(A);
    Uart_t uart0 = UART_Init(portA, UART0);
    Debug_Init(uart0);

    for(uint8_t idx = 0; idx < 4; idx++) {
        Debug_SendFromList(idx);
    }
    Debug_SendMsg("All done! Blinking now.\r\n");
    while(1) {
        GPIO_Toggle(portF, LED_GREEN);
        Timer_Wait1ms(timer0, 500);
    }
}
