#[[***********************************************************************************************
File:           test/CMakeLists.txt
Description:    Adds executables for test scripts.
**************************************************************************************************  ]]

if(OPT_TESTING)

#[[***********************************************************************************************
# Driver Tests
**************************************************************************************************  ]]  

# Phase-Lock-Loop (PLL)
add_executable(Test_PLL.elf 
                    test_pll.c
                    ${PROJECT_SOURCE_DIR}/src/device/startup_gcc.c
                )

target_include_directories(Test_PLL.elf 
                            PRIVATE 
                                ${PROJECT_SOURCE_DIR}/src/drivers
                                ${PROJECT_SOURCE_DIR}/src/device
                            )

# SysTick Interrupts
add_executable(Test_SysTick_Int.elf 
                    test_systick_int.c 
                    ${PROJECT_SOURCE_DIR}/src/device/startup_gcc.c
                    )

target_include_directories(Test_SysTick_Int.elf 
                            PRIVATE 
                                ${PROJECT_SOURCE_DIR}/src/drivers
                                ${PROJECT_SOURCE_DIR}/src/device
                            )

# Timer 0
add_executable(Test_Timer0.elf 
                    test_timer0.c 
                    ${PROJECT_SOURCE_DIR}/src/device/startup_gcc.c
                    )

target_include_directories(Test_Timer0.elf
                            PRIVATE 
                                ${PROJECT_SOURCE_DIR}/src/drivers
                                ${PROJECT_SOURCE_DIR}/src/device
                            )

# GPIO Interrupts
add_executable(Test_GPIO_Int.elf 
                    test_gpio_int.c 
                    ${PROJECT_SOURCE_DIR}/src/device/startup_gcc.c
                    )

target_include_directories(Test_GPIO_Int.elf 
                            PRIVATE 
                                ${PROJECT_SOURCE_DIR}/src/drivers
                                ${PROJECT_SOURCE_DIR}/src/device
                            )

# Timer 1 Interrupts
add_executable(Test_Timer1_Int.elf 
                    test_timer1_int.c 
                    ${PROJECT_SOURCE_DIR}/src/device/startup_gcc.c
                    )

target_include_directories(Test_Timer1_Int.elf
                            PRIVATE 
                                ${PROJECT_SOURCE_DIR}/src/drivers
                                ${PROJECT_SOURCE_DIR}/src/device
                            )

# UART
add_executable(Test_UART_Write.elf 
                    test_uart_Write.c 
                    ${PROJECT_SOURCE_DIR}/src/device/startup_gcc.c
                    )

target_include_directories(Test_UART_Write.elf
                            PRIVATE 
                                ${PROJECT_SOURCE_DIR}/src/drivers
                                ${PROJECT_SOURCE_DIR}/src/device
                            )

#*****************************************************************************
# Application SW Tests
#*****************************************************************************

# UserCtrl
add_executable(Test_UserCtrl.elf 
                    test_userctrl.c 
                    ${PROJECT_SOURCE_DIR}/src/drivers/isr.c
                    ${PROJECT_SOURCE_DIR}/src/device/startup_gcc.c
                    )

target_include_directories(Test_UserCtrl.elf 
                            PRIVATE 
                                ${PROJECT_SOURCE_DIR}/src/app
                                ${PROJECT_SOURCE_DIR}/src/drivers
                                ${PROJECT_SOURCE_DIR}/src/device
                            )

endif(OPT_TESTING)