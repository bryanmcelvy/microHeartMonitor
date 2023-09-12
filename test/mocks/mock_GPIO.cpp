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

GPIO_Port_t * GPIO_InitPort(GPIO_PortName_t portName) {
    return (GPIO_Port_t *) mock()
        .actualCall(__func__)
        .withParameter("portName", portName)
        .returnPointerValueOrDefault(0);
}

bool GPIO_isPortInit(GPIO_Port_t * gpioPort) {
    return mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .returnBoolValueOrDefault(true);
}

void GPIO_ConfigDirOutput(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}

void GPIO_ConfigDirInput(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}

void GPIO_ConfigPullUp(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}

void GPIO_ConfigPullDown(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}

void GPIO_ConfigDriveStrength(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask, uint8_t drive_mA) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask)
        .withParameter("drive_mA", drive_mA);
}

void GPIO_EnableDigital(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}

void GPIO_DisableDigital(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}

void GPIO_ConfigInterrupts_Edge(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask, bool risingEdge) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask)
        .withParameter("risingEdge", risingEdge);
}

void GPIO_ConfigInterrupts_BothEdges(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}

void GPIO_ConfigInterrupts_LevelTrig(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask, bool highLevel) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask)
        .withParameter("highLevel", highLevel);
}

void GPIO_ConfigNVIC(GPIO_Port_t * gpioPort, uint8_t priority) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("priority", priority);
}

uint8_t GPIO_ReadPins(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    return mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask)
        .returnUnsignedLongIntValueOrDefault(pinMask & 1);
}

void GPIO_WriteHigh(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}

void GPIO_WriteLow(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}

void GPIO_Toggle(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}

void GPIO_ConfigAltMode(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}

void GPIO_ConfigPortCtrl(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask, uint8_t fieldEncoding) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask)
        .withParameter("fieldEncoding", fieldEncoding);
}

void GPIO_ConfigAnalog(GPIO_Port_t * gpioPort, GPIO_Pin_t pinMask) {
    mock()
        .actualCall(__func__)
        .withParameter("gpioPort", gpioPort)
        .withParameter("pinMask", pinMask);
}
}
