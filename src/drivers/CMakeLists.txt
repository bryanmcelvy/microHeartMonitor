#**************************************************************************************************
# File:           /src/drivers/CMakeLists.txt
# Description:    Adds static libraries for low-level device drivers.
#**************************************************************************************************

# Include Directories and Link Libraries for all targets
include_directories(${PATH_COMMON} ${PATH_DEVICE} ${PATH_DRIVERS})

# Targets
add_library(ISR OBJECT ISR.c ISR.h)
target_link_libraries(ISR cmsis_core_header)

add_library(PLL STATIC PLL.c PLL.h)
target_link_libraries(PLL cmsis_core_header)

add_library(SysTick STATIC SysTick.c SysTick.h)

add_library(Timer STATIC Timer.c Timer.h)
target_link_libraries(Timer PRIVATE cmsis_core_header NewAssert)

add_library(GPIO STATIC GPIO.c GPIO.h)
target_link_libraries(GPIO PRIVATE cmsis_core_header NewAssert)

add_library(UART STATIC UART.c UART.h)
target_link_libraries(UART
                        PUBLIC GPIO
                        PRIVATE cmsis_core_header NewAssert)

add_library(SPI STATIC SPI.c SPI.h)
target_link_libraries(SPI
                        PUBLIC GPIO
                        PRIVATE cmsis_core_header NewAssert)

add_library(ADC STATIC ADC.c ADC.h)
target_include_directories(ADC PRIVATE ${CMSIS_DIRS})
target_link_libraries(ADC PRIVATE cmsis_core_header NewAssert GPIO)
