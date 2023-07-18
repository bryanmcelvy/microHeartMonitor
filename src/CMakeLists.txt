#[[***********************************************************************************************
File:           src/CMakeLists.txt
Description:    Adds subdirectories and main target executable; links static
                libraries to main target and test scripts
**************************************************************************************************  ]]
add_subdirectory(app)
add_subdirectory(common)
add_subdirectory(device)
add_subdirectory(drivers)
add_subdirectory(test)

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

#[[***********************************************************************************************
Library Linking (Driver Tests)
**************************************************************************************************  ]]

if(OPT_TESTING)

# PLL Test
target_link_libraries(  Test_PLL.elf 
                        PRIVATE 
                            PLL
                            SysTick
                        )

# SysTick Interrupt Test
target_link_libraries(  Test_SysTick_Int.elf 
                        PRIVATE 
                            PLL
                            SysTick
)

# Timer 0 Test
target_link_libraries(  Test_Timer0.elf
                        PRIVATE 
                            GPIO
                            PLL
                            Timer
)

# GPIO Interrupts Test
target_link_libraries(  Test_GPIO_Int.elf 
                        PRIVATE 
                            GPIO
                            PLL
                            SysTick
                            Timer
)

# Timer 1 Interrupts Test
target_link_libraries(  Test_Timer1_Int.elf
                        PRIVATE 
                            GPIO
                            PLL
                            Timer
)

# UART Test
target_link_libraries(  Test_UART_Write.elf
                        PRIVATE 
                            GPIO
                            PLL
                            Timer
                            UART
)

#[[***********************************************************************************************
Library Linking (App. Software Tests)
**************************************************************************************************  ]]

# UserCtrl Test
target_link_libraries(  Test_UserCtrl.elf 
                        PRIVATE 
                            UserCtrl
)

endif(OPT_TESTING)