#[[***********************************************************************************************
File:           /src/CMakeLists.txt
Description:    Adds subdirectories and main target executable
**************************************************************************************************  ]]

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
                                ${CMSIS_DIRS}
                            )
target_link_libraries(${MAIN_TARGET} Startup DAQ Debug LCD QRS PLL)
target_link_options(test_daq.elf PRIVATE "-Wl,-Map=main.map")
