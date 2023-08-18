/**
 * @file
 * @author  Bryan McElvy
 * @brief   Test script for Debug module.
 */

#include "Debug.h"

#include "PLL.h"
#include "GPIO.h"
#include "Timer.h"

#include <stdint.h>

int main(void) {
    PLL_Init();
    GPIO_PF_LED_Init();
    Timer0A_Init();

    Debug_Init();
    for(uint8_t idx = 0; idx < 4; idx++) {
        Debug_SendFromList(idx);
    }
    Debug_SendMsg("All done! Blinking now.\r\n");
    while(1) {
        GPIO_PF_LED_Toggle(LED_GREEN);
        Timer0A_Wait1ms(500);
    }
}
