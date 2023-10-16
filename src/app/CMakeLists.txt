#[[*********************************************************************************************
File:           /src/app/CMakeLists.txt
Description:    Adds application software libraries and link to driver libraries.
#***********************************************************************************************    ]]
include_directories(
                    ${PATH_COMMON}
                    ${PATH_DEVICE}
                    ${PATH_DRIVERS}
                )

add_library(DAQ STATIC DAQ.c DAQ_lookup.c DAQ.h)
target_include_directories(DAQ PRIVATE ${CMSIS_DIRS})
target_link_libraries(DAQ ADC CMSIS_DSP_IIR NewAssert Timer)

add_library(QRS STATIC QRS.c QRS.h)
target_include_directories(QRS PRIVATE ${CMSIS_DIRS})
target_link_libraries(QRS CMSIS_DSP_IIR CMSIS_DSP_FIR CMSIS_DSP_Math CMSIS_DSP_Stats)

add_library(LCD STATIC LCD.c LCD.h Font.c)
target_include_directories(LCD PRIVATE ${PATH_MIDDLEWARE})
target_link_libraries(LCD ILI9341)
