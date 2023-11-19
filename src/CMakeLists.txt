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

# Main (Bare-Metal)
set(MAIN_BARE_METAL main.elf)

add_executable(${MAIN_BARE_METAL} main.c)
target_include_directories( ${MAIN_BARE_METAL} 
                            PRIVATE 
                                ${PATH_APP}
                                ${PATH_COMMON}
                                ${PATH_DEVICE}
                                ${PATH_DRIVERS}
                                ${PATH_MIDDLEWARE}
                                ${PATH_CMSIS_INCLUDE})
target_link_libraries(${MAIN_BARE_METAL} Startup DAQ LCD QRS Debug Fifo GPIO ISR PLL UART)
target_link_options(${MAIN_BARE_METAL} PRIVATE "-Wl,-Map=main.map,--cref")

# Main (RTOS)
set(MAIN_RTOS main_rtos.elf)

add_executable(${MAIN_RTOS} main_rtos.c)
target_include_directories( ${MAIN_RTOS} 
                            PRIVATE 
                                ${PATH_APP}
                                ${PATH_COMMON}
                                ${PATH_DEVICE}
                                ${PATH_DRIVERS}
                                ${PATH_MIDDLEWARE}
                                ${PATH_CMSIS_INCLUDE}
                                ${FREERTOS_INC_DIRS})
target_link_libraries(${MAIN_RTOS}
                        Startup 
                        DAQ LCD QRS 
                        Debug 
                        Fifo 
                        GPIO ISR PLL UART 
                        freertos_list freertos_queue freertos_tasks freertos_list freertos_port)
target_link_options(${MAIN_RTOS} PRIVATE "-Wl,-Map=main_rtos.map,--cref")
