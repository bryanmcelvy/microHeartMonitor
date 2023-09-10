#[[*********************************************************************************************
File:           /src/app/CMakeLists.txt
Description:    Adds application software libraries and link to driver libraries.
#***********************************************************************************************    ]]
include_directories(
                    ${PATH_COMMON}
                    ${PATH_DEVICE}
                    ${PATH_DRIVERS}
                    ${PATH_MIDDLEWARE})

add_library(DAQ STATIC DAQ.c DAQ.h)
target_include_directories(DAQ PRIVATE ${CMSIS_DIRS})
target_link_libraries(DAQ ADC CMSIS-DSP-f32 FIFO)

add_library(QRS STATIC QRS.c QRS.h)
target_include_directories(QRS PRIVATE ${CMSIS_DIRS})
target_link_libraries(QRS CMSIS-DSP-f32 FIFO)

add_library(Debug STATIC Debug.c Debug.h)
target_include_directories(Debug PRIVATE ${CMSIS_DIRS})
target_link_libraries(Debug UART NewAssert)

add_library(LCD STATIC LCD.c LCD.h)
target_link_libraries(LCD ILI9341)

add_library(UserCtrl STATIC UserCtrl.c UserCtrl.h)
target_link_libraries(UserCtrl GPIO Timer)
