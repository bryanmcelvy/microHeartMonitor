#**************************************************************************************************
# File:           /src/drivers/CMakeLists.txt
# Description:    Adds static libraries for low-level device drivers.
#**************************************************************************************************

# Include Directories and Link Libraries for all targets
include_directories(${PATH_COMMON} ${PATH_DEVICE} ${PATH_DRIVERS})

# Targets
add_library(ISR OBJECT ISR.c ISR.h)
target_include_directories(ISR PUBLIC ${PATH_DEVICE})

add_library(PLL STATIC PLL.c PLL.h)

add_library(SysTick STATIC SysTick.c SysTick.h)

add_library(Timer STATIC Timer.c Timer.h)
target_link_libraries(Timer PUBLIC ISR NewAssert)

add_library(GPIO STATIC GPIO.c GPIO.h)
target_link_libraries(GPIO PUBLIC NewAssert)

add_library(UART STATIC UART.c UART.h)
target_link_libraries(UART PRIVATE GPIO)

add_library(SPI STATIC SPI.c SPI.h)
target_link_libraries(SPI PRIVATE GPIO)

add_library(ADC STATIC ADC.c ADC.h)
target_include_directories(ADC PRIVATE ${CMSIS_DIRS})
target_link_libraries(ADC PRIVATE GPIO Timer)
