#**************************************************************************************************
# File:           /external/CMakeLists.txt
# Description:    Builds selected functions from Arm's CMSIS-DSP library
#**************************************************************************************************

#**************************************************************************************************
# CMSIS-DSP
#**************************************************************************************************
add_compile_definitions(ARM_MATH_LOOPUNROLL)
add_compile_options("-Ofast")

add_library(CMSIS_DSP_IIR OBJECT
    ${PATH_CMSIS_SOURCE}/FilteringFunctions/arm_biquad_cascade_df1_f32.c    # IIR Filter
    ${PATH_CMSIS_INCLUDE}/dsp/filtering_functions.h
)
target_include_directories(CMSIS_DSP_IIR PUBLIC ${CMSIS_ALL_INCLUDE_DIRS})

add_library(CMSIS_DSP_FIR OBJECT
    ${PATH_CMSIS_SOURCE}/FilteringFunctions/arm_fir_f32.c                   # FIR Filter
    ${PATH_CMSIS_INCLUDE}/dsp/filtering_functions.h
)
target_include_directories(CMSIS_DSP_FIR PUBLIC ${CMSIS_ALL_INCLUDE_DIRS})

add_library(CMSIS_DSP_Math OBJECT
    ${PATH_CMSIS_SOURCE}/BasicMathFunctions/arm_mult_f32.c                  # Vector Multiplication
    ${PATH_CMSIS_INCLUDE}/dsp/basic_math_functions.h
)
target_include_directories(CMSIS_DSP_Math PUBLIC ${CMSIS_ALL_INCLUDE_DIRS})

add_library(CMSIS_DSP_Stats OBJECT
    ${PATH_CMSIS_SOURCE}/StatisticsFunctions/arm_max_f32.c                  # Max
    ${PATH_CMSIS_SOURCE}/StatisticsFunctions/arm_mean_f32.c                 # Mean
    ${PATH_CMSIS_INCLUDE}/dsp/statistics_functions.h
)
target_include_directories(CMSIS_DSP_Stats PUBLIC ${CMSIS_ALL_INCLUDE_DIRS})

add_library(CMSIS_DSP_Support OBJECT
    ${PATH_CMSIS_SOURCE}/SupportFunctions/arm_copy_f32.c                    # Vector Copy
    ${PATH_CMSIS_INCLUDE}/dsp/support_functions.h
)
target_include_directories(CMSIS_DSP_Support PUBLIC ${CMSIS_ALL_INCLUDE_DIRS})

#**************************************************************************************************
# FreeRTOS
#**************************************************************************************************

list(APPEND FREERTOS_SOURCE_FILES
                ${PATH_FREERTOS_SOURCE}/tasks.c
                ${PATH_FREERTOS_SOURCE}/queue.c
                ${PATH_FREERTOS_SOURCE}/list.c
                ${PATH_FREERTOS_SOURCE}/timers.c
                ${PATH_FREERTOS_SOURCE}/event_groups.c)

add_library(FreeRTOS OBJECT ${FREERTOS_SOURCE_FILES})
target_include_directories(FreeRTOS PRIVATE
                                        ${PATH_FREERTOS}
                                        ${PATH_FREERTOS_INCLUDE} 
                                        ${PATH_FREERTOS_SOURCE}/portable/GCC/ARM_CM4F
                                        ${PATH_DEVICE})
