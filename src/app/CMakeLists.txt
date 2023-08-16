#[[*********************************************************************************************
File:           src/app/CMakeLists.txt
Description:    Adds application software libraries and link to driver libraries.
#***********************************************************************************************    ]]
include_directories(${CMAKE_SOURCE_DIR}/src/drivers
                    ${CMAKE_SOURCE_DIR}/src/common
                    ${CMAKE_SOURCE_DIR}/src/device)

add_library(DAQ STATIC DAQ.c DAQ.h)
target_link_libraries(DAQ PRIVATE ADC CMSIS-DSP-f32 FIFO)
target_include_directories(DAQ PRIVATE ${CMSIS_DIRS})

add_library(QRS STATIC QRS.c QRS.h)
target_link_libraries(QRS PRIVATE CMSIS-DSP-f32 FIFO)
target_include_directories(QRS PRIVATE ${CMSIS_DIRS})

add_library(Debug STATIC Debug.c Debug.h)
target_link_libraries(Debug PRIVATE UART)

add_library(LCD STATIC LCD.c LCD.h)
target_link_libraries(LCD PRIVATE ILI9341)

add_library(UserCtrl STATIC UserCtrl.c UserCtrl.h)
target_link_libraries(UserCtrl 
                        PRIVATE 
                            GPIO 
                            Timer
                        )
