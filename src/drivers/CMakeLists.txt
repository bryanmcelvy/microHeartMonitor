#[[***********************************************************************************************
File:           /src/drivers/CMakeLists.txt
Description:    Adds static libraries for low-level device drivers and links as needed.
**************************************************************************************************  ]]
include_directories(${PATH_COMMON}
                    ${PATH_DEVICE})

link_libraries(NewAssert)

add_library(PLL STATIC PLL.c PLL.h)

add_library(SysTick STATIC SysTick.c SysTick.h)

add_library(Timer STATIC Timer.c Timer.h)

add_library(GPIO STATIC GPIO.c GPIO.h)

add_library(UART STATIC UART.c UART.h)
target_link_libraries(UART PRIVATE FIFO)

add_library(SPI STATIC SPI.c SPI.h)
target_link_libraries(SPI PRIVATE FIFO)

add_library(ADC STATIC ADC.c ADC.h)
target_link_libraries(ADC PRIVATE FIFO Lookup Timer)
target_include_directories(ADC PRIVATE ${CMSIS_DIRS})

add_library(ILI9341 STATIC ILI9341.c ILI9341.h)
target_link_libraries(ILI9341 PRIVATE SPI Timer)

add_library(GPIO_New STATIC GPIO_New.c GPIO_New.h)

