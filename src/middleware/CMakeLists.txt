#[[***********************************************************************************************
File:           /src/middleware/CMakeLists.txt
Description:    Adds static libraries for high-level device drivers.
**************************************************************************************************  ]]

# Include Directories and Link Libraries for all targets
include_directories(${PATH_COMMON} ${PATH_DEVICE} ${PATH_DRIVERS})

# Targets
add_library(Debug STATIC Debug.c Debug.h)
target_link_libraries(Debug UART NewAssert)

add_library(ILI9341 STATIC ILI9341.c ILI9341.h)
target_link_libraries(ILI9341 PRIVATE Fifo SPI Timer)

add_library(Led STATIC Led.c Led.h)
target_link_libraries(Led PRIVATE GPIO)
