#[[***********************************************************************************************
File:           /src/drivers/CMakeLists.txt
Description:    Adds static libraries for low-level device drivers and links as needed.
**************************************************************************************************  ]]

# Include Directories and Link Libraries for all targets
include_directories(${PATH_COMMON} ${PATH_DEVICE} ${PATH_DRIVERS})

# Targets
add_library(PLL STATIC PLL.c PLL.h)

add_library(SysTick STATIC SysTick.c SysTick.h)

add_library(Timer STATIC Timer.c Timer.h)

add_library(GPIO STATIC GPIO.c GPIO.h)

add_library(UART STATIC UART.c UART.h)
target_link_libraries(UART PRIVATE FIFO GPIO)

add_library(SPI STATIC SPI.c SPI.h)
target_link_libraries(SPI PRIVATE FIFO GPIO)

add_library(ADC STATIC ADC.c ADC.h)
target_link_libraries(ADC PRIVATE FIFO GPIO Lookup Timer)
target_include_directories(ADC PRIVATE ${CMSIS_DIRS})

add_library(ILI9341 STATIC ILI9341.c ILI9341.h)
target_link_libraries(ILI9341 PRIVATE SPI Timer)

add_library(LED STATIC Led.c Led.h)
target_link_libraries(LED PRIVATE GPIO_New)
