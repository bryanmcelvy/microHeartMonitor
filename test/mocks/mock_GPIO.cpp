//! @file
//! @copyright Copyright 2023. All Rights Reserved
//!
//! @details

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "GPIO.h"
#include <stdbool.h>
#include <stdint.h>

GpioPort_t GPIO_InitPort(GPIO_PortName_t portName) {
    return (GpioPort_t) mock()
        .actualCall(__func__)
        .withParameter("portName", portName)
        .returnPointerValueOrDefault(0);
}

bool GPIO_isPortInit(GpioPort_t gpioPort) {
    return mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .returnBoolValueOrDefault(true);
}

void GPIO_ConfigDirOutput(GpioPort_t gpioPort, GpioPin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}

void GPIO_ConfigDirInput(GpioPort_t gpioPort, GpioPin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}

void GPIO_ConfigPullUp(GpioPort_t gpioPort, GpioPin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}

void GPIO_ConfigPullDown(GpioPort_t gpioPort, GpioPin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}

void GPIO_ConfigDriveStrength(GpioPort_t gpioPort, GpioPin_t pinMask, uint8_t drive_mA) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask)
        .withParameter("drive_mA", drive_mA);
}

void GPIO_EnableDigital(GpioPort_t gpioPort, GpioPin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}

void GPIO_DisableDigital(GpioPort_t gpioPort, GpioPin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}

void GPIO_ConfigInterrupts_Edge(GpioPort_t gpioPort, GpioPin_t pinMask, bool risingEdge) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask)
        .withParameter("risingEdge", risingEdge);
}

void GPIO_ConfigInterrupts_BothEdges(GpioPort_t gpioPort, GpioPin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}

void GPIO_ConfigInterrupts_LevelTrig(GpioPort_t gpioPort, GpioPin_t pinMask, bool highLevel) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask)
        .withParameter("highLevel", highLevel);
}

void GPIO_ConfigNVIC(GpioPort_t gpioPort, uint8_t priority) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("priority", priority);
}

uint8_t GPIO_ReadPins(GpioPort_t gpioPort, GpioPin_t pinMask) {
    return mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask)
        .returnUnsignedLongIntValueOrDefault(pinMask & 1);
}

void GPIO_WriteHigh(GpioPort_t gpioPort, GpioPin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}

void GPIO_WriteLow(GpioPort_t gpioPort, GpioPin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}

void GPIO_Toggle(GpioPort_t gpioPort, GpioPin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}

void GPIO_ConfigAltMode(GpioPort_t gpioPort, GpioPin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}

void GPIO_ConfigPortCtrl(GpioPort_t gpioPort, GpioPin_t pinMask, uint8_t fieldEncoding) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask)
        .withParameter("fieldEncoding", fieldEncoding);
}

void GPIO_ConfigAnalog(GpioPort_t gpioPort, GpioPin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}
}
