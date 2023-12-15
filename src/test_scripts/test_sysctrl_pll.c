#include "sysctrl.h"
#include "GPIO.h"
#include "UART.h"

int main(void) {
    SysCtrl_PLL_Init(SYSCTRL_PLL_80MHZ);

    GpioPort_t portA = GPIO_InitPort(A);
    Uart_t uart0 = UART_Init(portA, UART0);
    int freq = (int) SysCtrl_getClkFreq_Hz();
    UART_WriteInt(uart0, freq);
    while(1) {}
}
