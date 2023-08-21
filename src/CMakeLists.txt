#[[***********************************************************************************************
File:           src/CMakeLists.txt
Description:    Adds subdirectories and main target executable
**************************************************************************************************  ]]
list(APPEND 
        CMSIS_DIRS                  
            "${CMAKE_SOURCE_DIR}/src/external/CMSIS-DSP/Include"
            "${CMAKE_SOURCE_DIR}/src/external/CMSIS-DSP/PrivateInclude"
            "${CMAKE_SOURCE_DIR}/src/external/CMSIS-DSP/Source"
            "${CMAKE_SOURCE_DIR}/src/external/CMSIS/Core/Include")

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
                    ${PROJECT_SOURCE_DIR}/src/device/startup_gcc.c
                )

target_include_directories( ${MAIN_TARGET} 
                            PRIVATE 
                                ${CMAKE_SOURCE_DIR}/src/app
                                ${CMAKE_SOURCE_DIR}/src/common
                                ${CMAKE_SOURCE_DIR}/src/device 
                                ${CMAKE_SOURCE_DIR}/src/drivers
                                ${CMSIS_DIRS}
                            )

target_link_libraries(  ${MAIN_TARGET} 
                        PRIVATE 
                            DAQ   Debug  LCD   QRS   UserCtrl       # app libraries
                            PLL                             # driver libraries
                        )
