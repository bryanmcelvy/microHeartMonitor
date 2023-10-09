// clang-format off
// NOLINTBEGIN

#include "CppUTest/TestHarness.h"

extern "C" {
#include "Led.h"
#include "GPIO.h"

#include <stdlib.h>
}

TEST_GROUP(Group_Led) { 
    GpioPort_t port;
    const GPIO_Pin_t pin = (GPIO_Pin_t) (1 << (rand() % 8));
    Led_t led;

    void setup() {
        port = GPIO_InitPort((GPIO_PortName_t) (rand() % 6));
        led = Led_Init(port, pin);
    } 

    void teardown()
    {
    } 
};

TEST(Group_Led, AfterInit_isOn) {
    CHECK_FALSE(Led_isOn(led));
}

TEST(Group_Led, AfterInit_GetPortReturnsCorrectPort) {
    GpioPort_t newPort = Led_GetPort(led);
    CHECK_EQUAL(port, newPort);
}

TEST(Group_Led, AfterInit_GetPinReturnsCorrectPin) {
    GPIO_Pin_t newPin = Led_GetPin(led);
    CHECK_EQUAL(pin, newPin);
}

TEST(Group_Led, AfterTurnOn_isOn) {
    Led_TurnOn(led);
    CHECK_TRUE(Led_isOn(led));
}

TEST(Group_Led, AfterToggleWhileOn_isOff) {
    Led_TurnOn(led);
    Led_Toggle(led);
    CHECK_FALSE(Led_isOn(led));
}

TEST(Group_Led, AfterTurnOff_isOff) {
    Led_TurnOff(led);
    CHECK_FALSE(Led_isOn(led));
}
TEST(Group_Led, AfterToggleWhileOff_isOn) {
    Led_TurnOff(led);
    Led_Toggle(led);
    CHECK_TRUE(Led_isOn(led));
}

// NOLINTEND
