#[[***********************************************************************************************
File:           test/ili9341/CMakeLists.txt
Description:    Adds executables for ILI9341-specific test scripts
**************************************************************************************************  ]]

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

# LCD Color Test
add_executable(test_lcd_color.elf 
                    test_lcd_color.c
                    ${PROJECT_SOURCE_DIR}/src/device/startup_gcc.c
                )

target_include_directories(test_lcd_color.elf 
                            PRIVATE 
                            ${PROJECT_SOURCE_DIR}/src/app
                            ${PROJECT_SOURCE_DIR}/src/drivers
                            ${PROJECT_SOURCE_DIR}/src/device
                            )

target_link_libraries(test_lcd_color.elf
                        PRIVATE
                            PLL
                            LCD
                            GPIO
                            Timer)
