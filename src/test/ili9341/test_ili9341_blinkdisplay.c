#include "PLL.h"
#include "ILI9341.h"

int main() {
    PLL_Init();
    Timer0A_Init();     // Timer2A is reserved for the ILI9341 module
    ILI9341_Init();

    while(1) {
        ILI9341_DisplayOn(1);
        Timer0A_Wait1ms(500);
        ILI9341_DisplayOn(0);
        Timer0A_Wait1ms(500);
    }
}