#[[***********************************************************************************************
File:           /src/drivers/CMakeLists.txt
Description:    Adds static libraries for low-level device drivers and links as needed.
**************************************************************************************************  ]]

# Include Directories and Link Libraries for all targets
include_directories(${PATH_COMMON} ${PATH_DEVICE} ${PATH_DRIVERS})

# Targets
add_library(ILI9341 STATIC ILI9341.c ILI9341.h)
target_link_libraries(ILI9341 PRIVATE SPI Timer)

add_library(LED STATIC Led.c Led.h)
target_link_libraries(LED PRIVATE GPIO)
