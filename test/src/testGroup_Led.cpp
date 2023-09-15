// clang-format off
// NOLINTBEGIN

#include "CppUTest/TestHarness.h"

extern "C" {
#include "Led.h"
#include "GPIO.h"
}

TEST_GROUP(Group_Led) { 
    GPIO_Port_t * port;
    Led_t * led;

    void setup() {
        port = GPIO_InitPort(B);
        led = Led_Init(port, GPIO_PIN0);
    } 

    void teardown()
    {
    } 
};

/******************************************************************************
Initialization
*******************************************************************************/

TEST(Group_Led, isOnAfterInit) {
    CHECK_FALSE(Led_isOn(led));
}

/******************************************************************************
Normal Functions
*******************************************************************************/

TEST(Group_Led, GetPortReturnsCorrectPort) {
    GPIO_Port_t * newPort = Led_GetPort(led);
    CHECK_EQUAL(port, newPort);
}

TEST(Group_Led, GetPinReturnsCorrectPin) {
    GPIO_Pin_t pin = Led_GetPin(led);
    CHECK_EQUAL(GPIO_PIN0, pin);
}

TEST(Group_Led, isOnAfterTurnOn) {
    Led_TurnOn(led);
    CHECK_TRUE(Led_isOn(led));
}

TEST(Group_Led, isOffAfterTurnOff) {
    Led_TurnOff(led);
    CHECK_FALSE(Led_isOn(led));
}

TEST(Group_Led, isOnAfterToggleAfterTurnOff) {
    Led_TurnOff(led);
    Led_Toggle(led);
    CHECK_TRUE(Led_isOn(led));
}

TEST(Group_Led, isOffAfterToggleAfterTurnOn) {
    Led_TurnOn(led);
    Led_Toggle(led);
    CHECK_FALSE(Led_isOn(led));
}

// NOLINTEND
