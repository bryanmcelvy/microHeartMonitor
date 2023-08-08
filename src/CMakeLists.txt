#[[***********************************************************************************************
File:           src/CMakeLists.txt
Description:    Adds subdirectories and main target executable
**************************************************************************************************  ]]

# Subdirectories
add_subdirectory(app)
add_subdirectory(common)
add_subdirectory(drivers)
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
                            )

target_link_libraries(  ${MAIN_TARGET} 
                        PRIVATE 
                            Debug   Filter  LCD   QRS   UserCtrl    # app libraries
                            ADC     PLL                             # driver libraries
                        )
