#**************************************************************************************************
# File:           /src/test/CMakeLists.txt
# Description:    Adds executables for test scripts.
#**************************************************************************************************

if(${OPT_TESTING})
    set_property(DIRECTORY PROPERTY EXCLUDE_FROM_ALL FALSE)
else()
    set_property(DIRECTORY PROPERTY EXCLUDE_FROM_ALL TRUE)
endif(${OPT_TESTING})

# Directory-wide Properties
include_directories(
                    ${PATH_COMMON}
                    ${PATH_DEVICE}
                    ${PATH_DRIVERS}
                    ${PATH_MIDDLEWARE})
link_libraries(PLL Startup)

# FIFO Buffer
add_executable(test_fifo.elf test_fifo.c)
target_include_directories(test_fifo.elf PRIVATE ${PATH_COMMON})
target_link_libraries(test_fifo.elf Fifo GPIO Timer UART)

#[[***********************************************************************************************
# Driver Tests
**************************************************************************************************  ]]  

# Phase-Lock-Loop (PLL)
add_executable(test_pll.elf test_pll.c)
target_link_libraries(test_pll.elf SysTick)

# SysTick Interrupts
add_executable(test_systick_int.elf test_systick_int.c )
target_link_libraries(test_systick_int.elf SysTick)

# Timer 0
add_executable(test_timer0.elf test_timer0.c )
target_link_libraries(test_timer0.elf GPIO Timer)

# GPIO Interrupts
add_executable(test_gpio_int.elf test_gpio_int.c )
target_link_libraries(test_gpio_int.elf GPIO ISR SysTick Timer)

# Timer 1 Interrupts
add_executable(test_timer1_int.elf test_timer1_int.c )
target_link_libraries(test_timer1_int.elf GPIO Timer ISR)

# UART
add_executable(test_uart_write.elf test_uart_write.c )
target_link_libraries(test_uart_write.elf GPIO Led UART)

# UART (Logic Analyzer)
add_executable(test_uart_la.elf test_uart_la.c )
target_link_libraries(test_uart_la.elf Timer UART)

#[[ 
UART (Interrupt) 
add_executable(test_uart_interrupt.elf test_uart_interrupt.c )
target_include_directories(test_uart_interrupt.elf PRIVATE ${PATH_COMMON})
target_link_libraries(test_uart_interrupt.elf GPIO Timer UART)
#]]

# Blink Display Test
add_executable(test_ili9341_blinkdisplay.elf test_ili9341_blinkdisplay.c)
target_include_directories(test_ili9341_blinkdisplay.elf  PRIVATE ${PATH_COMMON} )
target_link_libraries(test_ili9341_blinkdisplay.elf ILI9341 GPIO)

# ADC Test
add_executable(test_adc.elf test_adc.c)
target_include_directories(test_adc.elf PRIVATE ${PATH_COMMON})
target_link_libraries(test_adc.elf ADC Fifo GPIO Timer)

# GPIO (New) Test
add_executable(test_gpio_new.elf test_gpio_new.c)
target_link_libraries(test_gpio_new.elf GPIO Led Timer)

# Timer 1 Interrupts (Interrupt Table Relocation)
add_executable(test_isr_relocate.elf test_isr_relocate.c )
target_link_libraries(test_isr_relocate.elf Led Timer ISR)

# FreeRTOS Blink
add_executable(test_freertos_blink.elf test_freertos_blink.c ${PATH_MIDDLEWARE}/Led.c)
target_include_directories(test_freertos_blink.elf PRIVATE ${FREERTOS_INC_DIRS})
target_link_libraries(test_freertos_blink.elf Led freertos_tasks freertos_list freertos_port)

#*****************************************************************************
# Application SW Tests
#*****************************************************************************

#[[
LCD Color Test
add_executable(test_lcd_scroll.elf test_lcd_scroll.c)
target_include_directories(test_lcd_scroll.elf PRIVATE ${PATH_APP})
target_link_libraries(test_lcd_scroll.elf LCD GPIO Timer)
#]]

# LCD Image Test
add_executable(test_lcd_image.elf test_lcd_image.c)
target_include_directories(test_lcd_image.elf PRIVATE ${PATH_APP})
target_link_libraries(test_lcd_image.elf LCD GPIO Timer)

# DAQ Test
add_executable(test_daq.elf test_daq.c)
target_include_directories(test_daq.elf PRIVATE ${PATH_APP} ${PATH_CMSIS_INCLUDE})
target_link_libraries(test_daq.elf DAQ Debug Fifo ISR LCD)
target_link_options(test_daq.elf PRIVATE "-Wl,-Map=test_daq.map")

# Debug Module Test
add_executable(test_debug.elf test_debug.c)
target_include_directories(test_debug.elf PRIVATE ${PATH_APP} ${PATH_CMSIS_INCLUDE})
target_link_libraries(test_debug.elf Debug GPIO Timer)

# QRS Test
add_executable(test_qrs.elf test_qrs.c)
target_include_directories(test_qrs.elf PRIVATE ${PATH_APP} ${PATH_CMSIS_INCLUDE})
target_link_libraries(test_qrs.elf DAQ Debug ISR LCD QRS)

# LED Letter Test
add_executable(test_lcd_letters.elf test_lcd_letters.c)
target_include_directories(test_lcd_letters.elf PRIVATE ${PATH_APP})
target_link_libraries(test_lcd_letters.elf LCD Led Timer)
