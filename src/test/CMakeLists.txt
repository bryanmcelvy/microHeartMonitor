#[[***********************************************************************************************
File:           src/test/CMakeLists.txt
Description:    Adds executables for test scripts.
**************************************************************************************************  ]]

# FIFO Buffer
add_executable(test_fifo.elf 
                    test_fifo.c
                    ${PROJECT_SOURCE_DIR}/src/device/startup_gcc.c
                )

target_include_directories(test_fifo.elf 
                            PRIVATE 
                                ${PROJECT_SOURCE_DIR}/src/common
                                ${PROJECT_SOURCE_DIR}/src/drivers
                                ${PROJECT_SOURCE_DIR}/src/device
                            )

target_link_libraries(  test_fifo.elf 
                        PRIVATE 
                            FIFO
                            GPIO
                            PLL
                            Timer
                            UART
                        )

#[[***********************************************************************************************
# Driver Tests
**************************************************************************************************  ]]  

# Phase-Lock-Loop (PLL)
add_executable(test_pll.elf 
                    test_pll.c
                    ${PROJECT_SOURCE_DIR}/src/device/startup_gcc.c
                )

target_include_directories(test_pll.elf 
                            PRIVATE 
                                ${PROJECT_SOURCE_DIR}/src/drivers
                                ${PROJECT_SOURCE_DIR}/src/device
                            )

target_link_libraries(  test_pll.elf 
                        PRIVATE 
                            PLL
                            SysTick
                        )

# SysTick Interrupts
add_executable(test_systick_int.elf 
                    test_systick_int.c 
                    ${PROJECT_SOURCE_DIR}/src/device/startup_gcc.c
                    )

target_include_directories(test_systick_int.elf 
                            PRIVATE 
                                ${PROJECT_SOURCE_DIR}/src/drivers
                                ${PROJECT_SOURCE_DIR}/src/device
                            )

target_link_libraries(  test_systick_int.elf 
                        PRIVATE 
                            PLL
                            SysTick
)

# Timer 0
add_executable(test_timer0.elf 
                    test_timer0.c 
                    ${PROJECT_SOURCE_DIR}/src/device/startup_gcc.c
                    )

target_include_directories(test_timer0.elf
                            PRIVATE 
                                ${PROJECT_SOURCE_DIR}/src/drivers
                                ${PROJECT_SOURCE_DIR}/src/device
                            )

target_link_libraries(  test_timer0.elf
                        PRIVATE 
                            GPIO
                            PLL
                            Timer
)

# GPIO Interrupts
add_executable(test_gpio_int.elf 
                    test_gpio_int.c 
                    ${PROJECT_SOURCE_DIR}/src/device/startup_gcc.c
                    )

target_include_directories(test_gpio_int.elf 
                            PRIVATE 
                                ${PROJECT_SOURCE_DIR}/src/drivers
                                ${PROJECT_SOURCE_DIR}/src/device
                            )

target_link_libraries(  test_gpio_int.elf 
                        PRIVATE 
                            GPIO
                            PLL
                            SysTick
                            Timer
)

# Timer 1 Interrupts
add_executable(test_timer1_int.elf 
                    test_timer1_int.c 
                    ${PROJECT_SOURCE_DIR}/src/device/startup_gcc.c
                    )

target_include_directories(test_timer1_int.elf
                            PRIVATE 
                                ${PROJECT_SOURCE_DIR}/src/drivers
                                ${PROJECT_SOURCE_DIR}/src/device
                            )

target_link_libraries(  test_timer1_int.elf
                        PRIVATE 
                            GPIO
                            PLL
                            Timer
)

# UART
add_executable(test_uart_write.elf 
                    test_uart_write.c 
                    ${PROJECT_SOURCE_DIR}/src/device/startup_gcc.c
                    )

target_include_directories(test_uart_write.elf
                            PRIVATE 
                                ${PROJECT_SOURCE_DIR}/src/drivers
                                ${PROJECT_SOURCE_DIR}/src/device
                            )

target_link_libraries(  test_uart_write.elf
                        PRIVATE 
                            GPIO
                            PLL
                            Timer
                            UART
)

# UART (Logic Analyzer)
add_executable(test_uart_la.elf 
                    test_uart_la.c 
                    ${PROJECT_SOURCE_DIR}/src/device/startup_gcc.c
                    )

target_include_directories(test_uart_la.elf
                            PRIVATE 
                                ${PROJECT_SOURCE_DIR}/src/drivers
                                ${PROJECT_SOURCE_DIR}/src/device
                            )

target_link_libraries(  test_uart_la.elf
                        PRIVATE 
                            PLL
                            Timer
                            UART
)

# Blink Display Test
add_executable(test_ili9341_blinkdisplay.elf 
                    test_ili9341_blinkdisplay.c
                    ${PROJECT_SOURCE_DIR}/src/device/startup_gcc.c
                )

target_include_directories(test_ili9341_blinkdisplay.elf 
                            PRIVATE 
                                ${PROJECT_SOURCE_DIR}/src/drivers
                                ${PROJECT_SOURCE_DIR}/src/device
                            )

target_link_libraries(test_ili9341_blinkdisplay.elf
                        PRIVATE
                            PLL
                            ILI9341
                            GPIO)

#*****************************************************************************
# Application SW Tests
#*****************************************************************************

# LCD Color Test
add_executable(test_lcd_scroll.elf 
                    test_lcd_scroll.c
                    ${PROJECT_SOURCE_DIR}/src/device/startup_gcc.c
                )

target_include_directories(test_lcd_scroll.elf 
                            PRIVATE 
                            ${PROJECT_SOURCE_DIR}/src/app
                            ${PROJECT_SOURCE_DIR}/src/drivers
                            ${PROJECT_SOURCE_DIR}/src/device
                            )

target_link_libraries(test_lcd_scroll.elf
                        PRIVATE
                            PLL
                            LCD
                            GPIO
                            Timer)

# LCD Image Test
add_executable(test_lcd_image.elf 
                    test_lcd_image.c
                    ${PROJECT_SOURCE_DIR}/src/device/startup_gcc.c
                )

target_include_directories(test_lcd_image.elf 
                            PRIVATE 
                            ${PROJECT_SOURCE_DIR}/src/app
                            ${PROJECT_SOURCE_DIR}/src/drivers
                            ${PROJECT_SOURCE_DIR}/src/device
                            )

target_link_libraries(test_lcd_image.elf
                        PRIVATE
                            PLL
                            LCD
                            GPIO
                            Timer)
