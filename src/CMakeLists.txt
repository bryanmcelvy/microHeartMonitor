#[[***********************************************************************************************
File:           /src/CMakeLists.txt
Description:    Adds subdirectories and main target executable
**************************************************************************************************  ]]

# Subdirectories
add_subdirectory(app)
add_subdirectory(common)
add_subdirectory(drivers)
add_subdirectory(external/CMSIS-DSP)
add_subdirectory(test)

# Main Target Declaration
set(MAIN_TARGET main.elf)

add_executable(${MAIN_TARGET} 
                    main.c
                    ${PATH_DEVICE}/startup_gcc.c
                )

target_include_directories( ${MAIN_TARGET} 
                            PRIVATE 
                                ${PATH_APP}
                                ${PATH_COMMON}
                                ${PATH_DEVICE}
                                ${PATH_DRIVERS}
                                ${CMSIS_DIRS}
                            )

target_link_libraries(  ${MAIN_TARGET} 
                        PRIVATE 
                            DAQ   Debug  LCD   QRS   UserCtrl       # app libraries
                            PLL                             # driver libraries
                        )
