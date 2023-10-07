#include "PLL.h"
#include "GPIO.h"
#include "Led.h"
#include "Timer.h"

int main(void) {
    PLL_Init();

    Timer_t timer = Timer_Init(TIMER0);
    Timer_setMode(timer, ONESHOT, UP);

    GPIO_Port_t * portF = GPIO_InitPort(F);
    Led_t led = Led_Init(portF, GPIO_PIN3);

    while(1) {
        Led_Toggle(led);
        Timer_Wait1ms(timer, 333);
    }
}
