#include "FreeRTOS.h"
#include "task.h"

#include "PLL.h"
#include "GPIO.h"
#include "Led.h"

#define STACK_SIZE      ((UBaseType_t) 100)

#define NUM_TICKS_RED   ((TickType_t) (configTICK_RATE_HZ * 1))
#define NUM_TICKS_GREEN ((TickType_t) (configTICK_RATE_HZ * 0.5))
#define NUM_TICKS_BLUE  ((TickType_t) (configTICK_RATE_HZ * 0.25))

static TaskHandle_t ToggleRedHandle = 0;
static void ToggleRedTask(void * parameters);
static StackType_t ToggleRedStack[STACK_SIZE] = { 0 };
static StaticTask_t ToggleRedTaskBuffer;

static TaskHandle_t ToggleGreenHandle = 0;
static void ToggleGreenTask(void * parameters);
static StackType_t ToggleGreenStack[STACK_SIZE] = { 0 };
static StaticTask_t ToggleGreenTaskBuffer;

static TaskHandle_t ToggleBlueHandle = 0;
static void ToggleBlueTask(void * parameters);
static StackType_t ToggleBlueStack[STACK_SIZE] = { 0 };
static StaticTask_t ToggleBlueTaskBuffer;

static Led_t redLed = 0;
static Led_t blueLed = 0;
static Led_t greenLed = 0;

int main(void) {
    static GpioPort_t portF = 0;

    PLL_Init();

    // Initialize LEDs
    portF = GPIO_InitPort(GPIO_PORT_F);
    redLed = Led_Init(portF, LED_RED);
    greenLed = Led_Init(portF, LED_GREEN);
    blueLed = Led_Init(portF, LED_BLUE);

    // Initialize FreeRTOS tasks and start scheduler
    ToggleRedHandle = xTaskCreateStatic(ToggleRedTask, "Toggle Red", STACK_SIZE, NULL, 1,
                                        ToggleRedStack, &ToggleRedTaskBuffer);
    ToggleBlueHandle = xTaskCreateStatic(ToggleBlueTask, "Toggle Blue", STACK_SIZE, NULL, 1,
                                         ToggleBlueStack, &ToggleBlueTaskBuffer);
    ToggleGreenHandle = xTaskCreateStatic(ToggleGreenTask, "Toggle Green", STACK_SIZE, NULL, 1,
                                          ToggleGreenStack, &ToggleGreenTaskBuffer);

    vTaskStartScheduler();
    while(1) {}
}

void ToggleRedTask(void * parameters) {
    while(1) {
        Led_Toggle(redLed);
        vTaskDelay(NUM_TICKS_RED);
    }
}

void ToggleGreenTask(void * parameters) {
    while(1) {
        Led_Toggle(greenLed);
        vTaskDelay(NUM_TICKS_GREEN);
    }
}

void ToggleBlueTask(void * parameters) {
    while(1) {
        Led_Toggle(blueLed);
        vTaskDelay(NUM_TICKS_BLUE);
    }
}

void vApplicationTickHook(void) {}
