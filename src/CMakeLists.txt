#[[***********************************************************************************************
File:           src/CMakeLists.txt
Description:    Adds subdirectories and main target executable
**************************************************************************************************  ]]
add_subdirectory(app)
add_subdirectory(common)
add_subdirectory(device)
add_subdirectory(drivers)

if(OPT_TESTING)
    add_subdirectory(test)
endif(OPT_TESTING)


#[[***********************************************************************************************
Main Target Declaration
**************************************************************************************************  ]]
set(MAIN_TARGET ${CMAKE_PROJECT_NAME}.elf)

add_executable(${MAIN_TARGET} 
                    main.c
                    # ${PROJECT_SOURCE_DIR}/src/drivers/isr.c
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
                            Debug   Filter      QRS     UserCtrl    # app libraries
                            ADC     ILI9341     PLL                 # driver libraries
                        )
