/**
 * @file
 * @author  Bryan McElvy
 * @brief   Stub functions for GPIO module.
 */

// NOLINTBEGIN

#ifdef __cplusplus
extern "C" {
#endif

#include "GPIO.h"

#include "stdlib.h"

GPIO_Port_t * GPIO_InitPort(GPIO_PortName_t portName) {
    return 0;
}

bool GPIO_isPortInit(GPIO_Port_t * gpioPort) {
    return true;
}

void GPIO_ConfigDirOutput(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    return;
}

void GPIO_ConfigDirInput(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    return;
}

void GPIO_ConfigPullUp(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    return;
}

void GPIO_ConfigPullDown(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    return;
}

void GPIO_ConfigDriveStrength(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask, uint8_t drive_mA) {
    return;
}

void GPIO_EnableDigital(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    return;
}

void GPIO_DisableDigital(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    return;
}

void GPIO_ConfigInterrupts_Edge(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask, bool risingEdge) {
    return;
}

void GPIO_ConfigInterrupts_BothEdges(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    return;
}

void GPIO_ConfigInterrupts_LevelTrig(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask, bool highLevel) {
    return;
}

void GPIO_ConfigNVIC(GPIO_Port_t * gpioPort, uint8_t priority) {
    return;
}

uint8_t GPIO_ReadPins(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    return (rand() % 16) & pinMask;
}

void GPIO_WriteHigh(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    return;
}

void GPIO_WriteLow(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    return;
}

void GPIO_Toggle(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    return;
}

void GPIO_ConfigAltMode(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    return;
}

void GPIO_ConfigPortCtrl(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask, uint8_t fieldEncoding) {
    return;
}

void GPIO_ConfigAnalog(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    return;
}

#ifdef __cplusplus
}
#endif

// NOLINTEND
