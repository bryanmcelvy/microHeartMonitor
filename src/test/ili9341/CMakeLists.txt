#[[***********************************************************************************************
File:           test/ili9341/CMakeLists.txt
Description:    Adds executables for ILI9341-specific test scripts
**************************************************************************************************  ]]

# Blink Display Test
add_executable(Test_ILI9341_BlinkDisplay.elf 
                    test_ili9341_blinkdisplay.c
                    ${PROJECT_SOURCE_DIR}/src/device/startup_gcc.c
                )

target_include_directories(Test_ILI9341_BlinkDisplay.elf 
                            PRIVATE 
                                ${PROJECT_SOURCE_DIR}/src/drivers
                                ${PROJECT_SOURCE_DIR}/src/device
                            )

target_link_libraries(Test_ILI9341_BlinkDisplay.elf
                        PRIVATE
                            PLL
                            ILI9341
                            GPIO)

# LCD Color Test
add_executable(Test_LCD_Color.elf 
                    test_lcd_color.c
                    ${PROJECT_SOURCE_DIR}/src/device/startup_gcc.c
                )

target_include_directories(Test_LCD_Color.elf 
                            PRIVATE 
                            ${PROJECT_SOURCE_DIR}/src/app
                            ${PROJECT_SOURCE_DIR}/src/drivers
                            ${PROJECT_SOURCE_DIR}/src/device
                            )

target_link_libraries(Test_LCD_Color.elf
                        PRIVATE
                            PLL
                            LCD
                            GPIO
                            Timer)
