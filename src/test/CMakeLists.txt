#[[***********************************************************************************************
File:           /src/test/CMakeLists.txt
Description:    Adds executables for test scripts.
**************************************************************************************************  ]]

if(${OPT_TESTING})
set_property(DIRECTORY PROPERTY EXCLUDE_FROM_ALL TRUE)
endif(${OPT_TESTING})

link_libraries(PLL Startup)

# FIFO Buffer
add_executable(test_fifo.elf test_fifo.c)

target_include_directories(test_fifo.elf 
                            PRIVATE 
                                ${PATH_COMMON}
                                ${PATH_DRIVERS}
                                ${PATH_DEVICE}
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
add_executable(test_pll.elf test_pll.c)

target_include_directories(test_pll.elf 
                            PRIVATE 
                                ${PATH_DRIVERS}
                                ${PATH_DEVICE}
                            )

target_link_libraries(  test_pll.elf 
                        PRIVATE 
                            PLL
                            SysTick
                        )

# SysTick Interrupts
add_executable(test_systick_int.elf test_systick_int.c )

target_include_directories(test_systick_int.elf 
                            PRIVATE 
                                ${PATH_DRIVERS}
                                ${PATH_DEVICE}
                            )

target_link_libraries(  test_systick_int.elf 
                        PRIVATE 
                            PLL
                            SysTick
)

# Timer 0
add_executable(test_timer0.elf test_timer0.c )

target_include_directories(test_timer0.elf
                            PRIVATE 
                                ${PATH_DRIVERS}
                                ${PATH_DEVICE}
                            )

target_link_libraries(  test_timer0.elf
                        PRIVATE 
                            GPIO
                            PLL
                            Timer
)

# GPIO Interrupts
add_executable(test_gpio_int.elf test_gpio_int.c )

target_include_directories(test_gpio_int.elf 
                            PRIVATE 
                                ${PATH_DRIVERS}
                                ${PATH_DEVICE}
                            )

target_link_libraries(  test_gpio_int.elf 
                        PRIVATE 
                            GPIO
                            PLL
                            SysTick
                            Timer
)

# Timer 1 Interrupts
add_executable(test_timer1_int.elf test_timer1_int.c )

target_include_directories(test_timer1_int.elf
                            PRIVATE 
                                ${PATH_DRIVERS}
                                ${PATH_DEVICE}
                            )

target_link_libraries(  test_timer1_int.elf
                        PRIVATE 
                            GPIO
                            PLL
                            Timer
)

# UART
add_executable(test_uart_write.elf test_uart_write.c )

target_include_directories(test_uart_write.elf
                            PRIVATE 
                                ${PATH_DRIVERS}
                                ${PATH_DEVICE}
                            )

target_link_libraries(  test_uart_write.elf
                        PRIVATE 
                            GPIO
                            PLL
                            Timer
                            UART
)

# UART (Logic Analyzer)
add_executable(test_uart_la.elf test_uart_la.c )

target_include_directories(test_uart_la.elf
                            PRIVATE 
                                ${PATH_DRIVERS}
                                ${PATH_DEVICE}
                            )

target_link_libraries(  test_uart_la.elf
                        PRIVATE 
                            PLL
                            Timer
                            UART
)

# UART (Logic Analyzer)
add_executable(test_uart_interrupt.elf test_uart_interrupt.c )

target_include_directories(test_uart_interrupt.elf
                            PRIVATE
                                ${PATH_COMMON}
                                ${PATH_DEVICE}
                                ${PATH_DRIVERS}
                            )

target_link_libraries(  test_uart_interrupt.elf
                        PRIVATE 
                            PLL
                            GPIO
                            Timer
                            UART
)

# Blink Display Test
add_executable(test_ili9341_blinkdisplay.elf test_ili9341_blinkdisplay.c)

target_include_directories(test_ili9341_blinkdisplay.elf 
                            PRIVATE 
                                ${PATH_COMMON}
                                ${PATH_DRIVERS}
                                ${PATH_DEVICE}
                            )

target_link_libraries(test_ili9341_blinkdisplay.elf
                        PRIVATE
                            PLL
                            ILI9341
                            GPIO)

# ADC Test
add_executable(test_adc.elf test_adc.c)

target_include_directories(test_adc.elf 
                            PRIVATE 
                                ${PATH_COMMON}
                                ${PATH_DRIVERS}
                                ${PATH_DEVICE}
                            )

target_link_libraries(test_adc.elf
                        PRIVATE
                            PLL
                            ADC
                            FIFO
                            GPIO
                            Timer
                            # UART
                        )

#*****************************************************************************
# Application SW Tests
#*****************************************************************************

# LCD Color Test
add_executable(test_lcd_scroll.elf test_lcd_scroll.c)

target_include_directories(test_lcd_scroll.elf 
                            PRIVATE 
                            ${PATH_APP}
                            ${PATH_COMMON}
                            ${PATH_DRIVERS}
                            ${PATH_DEVICE}
                            )

target_link_libraries(test_lcd_scroll.elf
                        PRIVATE
                            PLL
                            LCD
                            GPIO
                            Timer)

target_compile_definitions(test_lcd_scroll.elf
                            PUBLIC 
                                SPI_USING_INTERRUPTS=true
                                FIFO_POOL_SIZE=1
)

# LCD Image Test
add_executable(test_lcd_image.elf test_lcd_image.c)

target_include_directories(test_lcd_image.elf 
                            PRIVATE 
                            ${PATH_APP}
                            ${PATH_COMMON}
                            ${PATH_DRIVERS}
                            ${PATH_DEVICE}
                            )

target_link_libraries(test_lcd_image.elf
                        PRIVATE
                            PLL
                            LCD
                            GPIO
                            Timer)

# DAQ Test
add_executable(test_daq.elf test_daq.c)

target_include_directories(test_daq.elf 
                            PRIVATE 
                            ${PATH_APP}
                            ${PATH_COMMON}
                            ${PATH_DRIVERS}
                            ${PATH_DEVICE}
                            ${CMSIS_DIRS}
                            )

target_link_libraries(test_daq.elf
                        PRIVATE
                            DAQ
                            Debug
                            FIFO
                            LCD
                            PLL
                            )

# Debug Module Test
add_executable(test_debug.elf test_debug.c)

target_include_directories(test_debug.elf
                            PRIVATE
                            ${PATH_APP}
                            ${PATH_COMMON}
                            ${PATH_DRIVERS}
                            ${PATH_DEVICE}
                            ${CMSIS_DIRS}
                            )

target_link_libraries(test_debug.elf
                        PRIVATE
                            Debug
                            PLL
                            GPIO
                            Timer)
