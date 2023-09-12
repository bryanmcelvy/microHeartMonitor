// clang-format off
// NOLINTBEGIN

#include "CppUTest/TestHarness.h"

extern "C" {
#include "Led.h"
#include "GPIO.h"
}

/******************************************************************************
Initialization
*******************************************************************************/

TEST_GROUP(Group_LedInit) { 
    GPIO_Port_t * port;

    void setup() {
        port = GPIO_InitPort(A);
        GPIO_ConfigDirOutput(port, GPIO_PIN0);
        GPIO_EnableDigital(port, GPIO_PIN0);
    } 

    void teardown()
    {
    } 
};

TEST(Group_LedInit, Test_Init) {
    Led_t * led = Led_Init(port, GPIO_PIN0);
    CHECK_FALSE(Led_isOn(led));
}

/******************************************************************************
Normal Functions
*******************************************************************************/

TEST_GROUP(Group_Led) { 
    GPIO_Port_t * port;
    Led_t * led;

    void setup() {
        port = GPIO_InitPort(B);
        GPIO_ConfigDirOutput(port, GPIO_PIN0);
        GPIO_EnableDigital(port, GPIO_PIN0);

        led = Led_Init(port, GPIO_PIN0);
    } 

    void teardown()
    {
    } 
};

TEST(Group_Led, Test_GetPortReturnsCorrectPort) {
    GPIO_Port_t * newPort = Led_GetPort(led);
    CHECK_EQUAL(port, newPort);
}

TEST(Group_Led, Test_GetPinReturnsCorrectPin) {
    GPIO_Pin_t pin = Led_GetPin(led);
    CHECK_EQUAL(GPIO_PIN0, pin);
}

TEST(Group_Led, Test_isOnAfterTurnOn) {
    Led_TurnOn(led);
    CHECK_TRUE(Led_isOn(led));
}

TEST(Group_Led, Test_isOffAfterTurnOff) {
    Led_TurnOff(led);
    CHECK_FALSE(Led_isOn(led));
}

TEST(Group_Led, Test_isOnAfterToggleAfterTurnOff) {
    Led_TurnOff(led);
    Led_Toggle(led);
    CHECK_TRUE(Led_isOn(led));
}

TEST(Group_Led, Test_isOffAfterToggleAfterTurnOn) {
    Led_TurnOn(led);
    Led_Toggle(led);
    CHECK_FALSE(Led_isOn(led));
}

// NOLINTEND
