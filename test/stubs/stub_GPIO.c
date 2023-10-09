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

GpioPort_t GPIO_InitPort(GPIO_PortName_t portName) {
    return 0;
}

bool GPIO_isPortInit(GpioPort_t gpioPort) {
    return true;
}

void GPIO_ConfigDirOutput(GpioPort_t gpioPort, GPIO_Pin_t pinMask) {
    return;
}

void GPIO_ConfigDirInput(GpioPort_t gpioPort, GPIO_Pin_t pinMask) {
    return;
}

void GPIO_ConfigPullUp(GpioPort_t gpioPort, GPIO_Pin_t pinMask) {
    return;
}

void GPIO_ConfigPullDown(GpioPort_t gpioPort, GPIO_Pin_t pinMask) {
    return;
}

void GPIO_ConfigDriveStrength(GpioPort_t gpioPort, GPIO_Pin_t pinMask, uint8_t drive_mA) {
    return;
}

void GPIO_EnableDigital(GpioPort_t gpioPort, GPIO_Pin_t pinMask) {
    return;
}

void GPIO_DisableDigital(GpioPort_t gpioPort, GPIO_Pin_t pinMask) {
    return;
}

void GPIO_ConfigInterrupts_Edge(GpioPort_t gpioPort, GPIO_Pin_t pinMask, bool risingEdge) {
    return;
}

void GPIO_ConfigInterrupts_BothEdges(GpioPort_t gpioPort, GPIO_Pin_t pinMask) {
    return;
}

void GPIO_ConfigInterrupts_LevelTrig(GpioPort_t gpioPort, GPIO_Pin_t pinMask, bool highLevel) {
    return;
}

void GPIO_ConfigNVIC(GpioPort_t gpioPort, uint8_t priority) {
    return;
}

uint8_t GPIO_ReadPins(GpioPort_t gpioPort, GPIO_Pin_t pinMask) {
    return (rand() % 16) & pinMask;
}

void GPIO_WriteHigh(GpioPort_t gpioPort, GPIO_Pin_t pinMask) {
    return;
}

void GPIO_WriteLow(GpioPort_t gpioPort, GPIO_Pin_t pinMask) {
    return;
}

void GPIO_Toggle(GpioPort_t gpioPort, GPIO_Pin_t pinMask) {
    return;
}

void GPIO_ConfigAltMode(GpioPort_t gpioPort, GPIO_Pin_t pinMask) {
    return;
}

void GPIO_ConfigPortCtrl(GpioPort_t gpioPort, GPIO_Pin_t pinMask, uint8_t fieldEncoding) {
    return;
}

void GPIO_ConfigAnalog(GpioPort_t gpioPort, GPIO_Pin_t pinMask) {
    return;
}

#ifdef __cplusplus
}
#endif

// NOLINTEND
