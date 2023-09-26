//! @file
//! @copyright Copyright 2023. All Rights Reserved
//!
//! @details

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "SPI.h"
#include <stdbool.h>
#include <stdint.h>
}

void SPI_Init(void) {
    mock().actualCall(__func__);
}

uint8_t SPI_Read(void) {
    return mock().actualCall(__func__).returnUnsignedLongIntValueOrDefault(0);
}

void SPI_WriteCmd(uint8_t cmd) {
    mock().actualCall(__func__).withParameter("cmd", cmd);
}

void SPI_WriteData(uint8_t data) {
    mock().actualCall(__func__).withParameter("data", data);
}
