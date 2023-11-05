#**************************************************************************************************
# File:           /src/CMakeLists.txt
# Description:    Adds subdirectories and main target executable
#**************************************************************************************************

# Add startup file as Object Library
add_library(Startup OBJECT ${PATH_DEVICE}/startup_gcc.c)

# Subdirectories
add_subdirectory(${PATH_APP})
add_subdirectory(${PATH_COMMON})
add_subdirectory(${PATH_DRIVERS})
add_subdirectory(${PATH_MIDDLEWARE})
add_subdirectory(${PATH_TEST_SCRIPTS})

# Main Target Declaration
set(MAIN_TARGET main.elf)

add_executable(${MAIN_TARGET} main.c)
target_include_directories( ${MAIN_TARGET} 
                            PRIVATE 
                                ${PATH_APP}
                                ${PATH_COMMON}
                                ${PATH_DEVICE}
                                ${PATH_DRIVERS}
                                ${PATH_MIDDLEWARE}
                                ${PATH_CMSIS_INCLUDE}
                            )
target_link_libraries(${MAIN_TARGET} Startup DAQ LCD QRS Debug Fifo GPIO ISR PLL UART)
target_link_options(${MAIN_TARGET} PRIVATE "-Wl,-Map=main.map,--cref")
