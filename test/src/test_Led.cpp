// clang-format off
// NOLINTBEGIN

#include "CppUTest/TestHarness.h"

extern "C" {
#include "Led.h"

#include "GPIO.h"
}

static GPIO_Port_t * port;

TEST_GROUP(GroupLedInit) { 
    void setup() {
        port = GPIO_InitPort(A);
        GPIO_ConfigDirOutput(port, GPIO_PIN0);
        GPIO_EnableDigital(port, GPIO_PIN0);
    } 

    void teardown()
    {

    } 
};

TEST(GroupLedInit, TestLedInit) {
    Led_t * led = Led_Init(port, GPIO_PIN0);
    CHECK_FALSE(Led_isOn(led));
}
