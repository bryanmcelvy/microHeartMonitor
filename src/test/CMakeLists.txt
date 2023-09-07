#[[***********************************************************************************************
File:           /src/test/CMakeLists.txt
Description:    Adds executables for test scripts.
**************************************************************************************************  ]]

if(${OPT_TESTING})
    set_property(DIRECTORY PROPERTY EXCLUDE_FROM_ALL FALSE)
else()
    set_property(DIRECTORY PROPERTY EXCLUDE_FROM_ALL TRUE)
endif(${OPT_TESTING})

# Include Directories and Link Libraries for all targets
include_directories(${PATH_COMMON} ${PATH_DEVICE} ${PATH_DRIVERS})
link_libraries(PLL Startup)

# FIFO Buffer
add_executable(test_fifo.elf test_fifo.c)
target_include_directories(test_fifo.elf PRIVATE ${PATH_COMMON})
target_link_libraries(test_fifo.elf PRIVATE FIFO GPIO Timer UART)

#[[***********************************************************************************************
# Driver Tests
**************************************************************************************************  ]]  

# Phase-Lock-Loop (PLL)
add_executable(test_pll.elf test_pll.c)
target_link_libraries(test_pll.elf PRIVATE SysTick)

# SysTick Interrupts
add_executable(test_systick_int.elf test_systick_int.c )
target_link_libraries(test_systick_int.elf PRIVATE SysTick)

# Timer 0
add_executable(test_timer0.elf test_timer0.c )
target_link_libraries(test_timer0.elf PRIVATE GPIO Timer)

# GPIO Interrupts
add_executable(test_gpio_int.elf test_gpio_int.c )
target_link_libraries(test_gpio_int.elf PRIVATE GPIO SysTick Timer)

# Timer 1 Interrupts
add_executable(test_timer1_int.elf test_timer1_int.c )
target_link_libraries(test_timer1_int.elf PRIVATE GPIO Timer)

# UART
add_executable(test_uart_write.elf test_uart_write.c )
target_link_libraries(test_uart_write.elf PRIVATE GPIO Timer UART)

# UART (Logic Analyzer)
add_executable(test_uart_la.elf test_uart_la.c )
target_link_libraries(test_uart_la.elf PRIVATE Timer UART)

# UART (Logic Analyzer)
add_executable(test_uart_interrupt.elf test_uart_interrupt.c )
target_include_directories(test_uart_interrupt.elf PRIVATE ${PATH_COMMON})
target_link_libraries(test_uart_interrupt.elf PRIVATE GPIO Timer UART)

# Blink Display Test
add_executable(test_ili9341_blinkdisplay.elf test_ili9341_blinkdisplay.c)
target_include_directories(test_ili9341_blinkdisplay.elf PRIVATE  ${PATH_COMMON} )
target_link_libraries(test_ili9341_blinkdisplay.elf PRIVATE ILI9341 GPIO)

# ADC Test
add_executable(test_adc.elf test_adc.c)
target_include_directories(test_adc.elf PRIVATE ${PATH_COMMON})
target_link_libraries(test_adc.elf PRIVATE ADC FIFO GPIO Timer)

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
target_include_directories(test_lcd_scroll.elf  PRIVATE
                            ${PATH_APP})
target_link_libraries(test_lcd_scroll.elf PRIVATE LCD GPIO Timer)

# LCD Image Test
add_executable(test_lcd_image.elf test_lcd_image.c)
target_include_directories(test_lcd_image.elf PRIVATE ${PATH_APP})
target_link_libraries(test_lcd_image.elf PRIVATE LCD GPIO Timer)

# DAQ Test
add_executable(test_daq.elf test_daq.c)
target_include_directories(test_daq.elf PRIVATE ${PATH_APP} ${CMSIS_DIRS})
target_link_libraries(test_daq.elf PRIVATE DAQ Debug FIFO LCD)

# Debug Module Test
add_executable(test_debug.elf test_debug.c)
target_include_directories(test_debug.elf PRIVATE ${PATH_APP} ${CMSIS_DIRS})
target_link_libraries(test_debug.elf PRIVATE Debug GPIO Timer)
