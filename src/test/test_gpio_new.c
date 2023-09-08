#include "PLL.h"
#include "GPIO.h"
#include "Led.h"
#include "Timer.h"

int main(void) {
    PLL_Init();
    Timer0A_Init();

    GPIO_Port_t * portF = GPIO_InitPort(F);
    Led_t * led = Led_Init(portF, GPIO_PIN3);

    while(1) {
        Led_Toggle(led);
        Timer0A_Wait1ms(333);
    }
}
