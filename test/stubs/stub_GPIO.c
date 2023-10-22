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

static struct {
    uint32_t data;
    bool isInit;
} GpioPortStub;

/******************************************************************************
Initialization
*******************************************************************************/

GpioPort_t GPIO_InitPort(GPIO_PortName_t portName) {
    return 0;
}

bool GPIO_isPortInit(GpioPort_t gpioPort) {
    return true;
}

/******************************************************************************
Configuration (Digital I/O)
*******************************************************************************/

void GPIO_ConfigDirOutput(GpioPort_t gpioPort, GpioPin_t pinMask) {
    return;
}

void GPIO_ConfigDirInput(GpioPort_t gpioPort, GpioPin_t pinMask) {
    return;
}

void GPIO_ConfigPullUp(GpioPort_t gpioPort, GpioPin_t pinMask) {
    return;
}

void GPIO_ConfigPullDown(GpioPort_t gpioPort, GpioPin_t pinMask) {
    return;
}

void GPIO_ConfigDriveStrength(GpioPort_t gpioPort, GpioPin_t pinMask, uint8_t drive_mA) {
    return;
}

void GPIO_EnableDigital(GpioPort_t gpioPort, GpioPin_t pinMask) {
    return;
}

void GPIO_DisableDigital(GpioPort_t gpioPort, GpioPin_t pinMask) {
    return;
}

/******************************************************************************
Configuration (Interrupts)
*******************************************************************************/

void GPIO_ConfigInterrupts_Edge(GpioPort_t gpioPort, GpioPin_t pinMask, bool risingEdge) {
    return;
}

void GPIO_ConfigInterrupts_BothEdges(GpioPort_t gpioPort, GpioPin_t pinMask) {
    return;
}

void GPIO_ConfigInterrupts_LevelTrig(GpioPort_t gpioPort, GpioPin_t pinMask, bool highLevel) {
    return;
}

void GPIO_ConfigNVIC(GpioPort_t gpioPort, uint8_t priority) {
    return;
}

uint8_t GPIO_ReadPins(GpioPort_t gpioPort, GpioPin_t pinMask) {
    return (rand() % 16) & pinMask;
}

/******************************************************************************
Basic Functions (Digital I/O)
*******************************************************************************/

volatile uint32_t * GPIO_getDataRegister(GpioPort_t gpioPort) {
    return &GpioPortStub.data;
}

void GPIO_WriteHigh(GpioPort_t gpioPort, GpioPin_t pinMask) {
    GpioPortStub.data |= pinMask;
    return;
}

void GPIO_WriteLow(GpioPort_t gpioPort, GpioPin_t pinMask) {
    GpioPortStub.data &= ~(pinMask);
    return;
}

void GPIO_Toggle(GpioPort_t gpioPort, GpioPin_t pinMask) {
    GpioPortStub.data ^= pinMask;
    return;
}

/******************************************************************************
Configuration (Alternate/Analog Modes)
*******************************************************************************/

void GPIO_ConfigAltMode(GpioPort_t gpioPort, GpioPin_t pinMask) {
    return;
}

void GPIO_ConfigPortCtrl(GpioPort_t gpioPort, GpioPin_t pinMask, uint8_t fieldEncoding) {
    return;
}

void GPIO_ConfigAnalog(GpioPort_t gpioPort, GpioPin_t pinMask) {
    return;
}

#ifdef __cplusplus
}
#endif

// NOLINTEND
