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
    Timer0A_Init();

    // Init. LED pins
    GPIO_Port_t * portF = GPIO_InitPort(F);
    GPIO_ConfigDirOutput(portF, (GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3));
    GPIO_ConfigDriveStrength(portF, (GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3), 8);
    GPIO_EnableDigital(portF, (GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3));

    Debug_Init();
    for(uint8_t idx = 0; idx < 4; idx++) {
        Debug_SendFromList(idx);
    }
    Debug_SendMsg("All done! Blinking now.\r\n");
    while(1) {
        GPIO_Toggle(portF, LED_GREEN);
        Timer0A_Wait1ms(500);
    }
}
