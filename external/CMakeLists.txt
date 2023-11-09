#**************************************************************************************************
# File:           /external/CMakeLists.txt
# Description:    Builds selected functions from Arm's CMSIS-DSP library
#**************************************************************************************************

#**************************************************************************************************
# CMSIS-DSP
#**************************************************************************************************
list(APPEND CMSIS_DSP_COMPILE_DEF "ARM_MATH_LOOPUNROLL")
list(APPEND CMSIS_DSP_COMPILE_OPT "-Ofast")

add_library(CMSIS_DSP_IIR OBJECT
    ${PATH_CMSIS_SOURCE}/FilteringFunctions/arm_biquad_cascade_df1_f32.c    # IIR Filter
    ${PATH_CMSIS_INCLUDE}/dsp/filtering_functions.h)
target_include_directories(CMSIS_DSP_IIR PUBLIC ${CMSIS_ALL_INCLUDE_DIRS})
target_compile_definitions(CMSIS_DSP_IIR PRIVATE ${CMSIS_DSP_COMPILE_DEF})
target_compile_options(CMSIS_DSP_IIR PRIVATE ${CMSIS_DSP_COMPILE_OPT})

add_library(CMSIS_DSP_FIR OBJECT
    ${PATH_CMSIS_SOURCE}/FilteringFunctions/arm_fir_f32.c                   # FIR Filter
    ${PATH_CMSIS_INCLUDE}/dsp/filtering_functions.h)
target_include_directories(CMSIS_DSP_FIR PUBLIC ${CMSIS_ALL_INCLUDE_DIRS})
target_compile_definitions(CMSIS_DSP_FIR PRIVATE ${CMSIS_DSP_COMPILE_DEF})
target_compile_options(CMSIS_DSP_FIR PRIVATE ${CMSIS_DSP_COMPILE_OPT})

add_library(CMSIS_DSP_Math OBJECT
    ${PATH_CMSIS_SOURCE}/BasicMathFunctions/arm_mult_f32.c                  # Vector Multiplication
    ${PATH_CMSIS_INCLUDE}/dsp/basic_math_functions.h)
target_include_directories(CMSIS_DSP_Math PUBLIC ${CMSIS_ALL_INCLUDE_DIRS})
target_compile_definitions(CMSIS_DSP_Math PRIVATE ${CMSIS_DSP_COMPILE_DEF})
target_compile_options(CMSIS_DSP_Math PRIVATE ${CMSIS_DSP_COMPILE_OPT})

add_library(CMSIS_DSP_Stats OBJECT
    ${PATH_CMSIS_SOURCE}/StatisticsFunctions/arm_max_f32.c                  # Max
    ${PATH_CMSIS_SOURCE}/StatisticsFunctions/arm_mean_f32.c                 # Mean
    ${PATH_CMSIS_INCLUDE}/dsp/statistics_functions.h)
target_include_directories(CMSIS_DSP_Stats PUBLIC ${CMSIS_ALL_INCLUDE_DIRS})
target_compile_definitions(CMSIS_DSP_Stats PRIVATE ${CMSIS_DSP_COMPILE_DEF})
target_compile_options(CMSIS_DSP_Stats PRIVATE ${CMSIS_DSP_COMPILE_OPT})

add_library(CMSIS_DSP_Support OBJECT
    ${PATH_CMSIS_SOURCE}/SupportFunctions/arm_copy_f32.c                    # Vector Copy
    ${PATH_CMSIS_INCLUDE}/dsp/support_functions.h)
target_include_directories(CMSIS_DSP_Support PUBLIC ${CMSIS_ALL_INCLUDE_DIRS})
target_compile_definitions(CMSIS_DSP_Support PRIVATE ${CMSIS_DSP_COMPILE_DEF})
target_compile_options(CMSIS_DSP_Support PRIVATE ${CMSIS_DSP_COMPILE_OPT})

#**************************************************************************************************
# FreeRTOS
#**************************************************************************************************
list(APPEND FREERTOS_HEADERS
                ${PATH_FREERTOS_INCLUDE}/FreeRTOS.h
                ${PATH_FREERTOS_INCLUDE}/portable.h)

# Port
add_library(freertos_port OBJECT ${PATH_FREERTOS_SOURCE}/portable/GCC/ARM_CM4F/port.c)
target_precompile_headers(freertos_port PRIVATE ${FREERTOS_HEADERS} ${PATH_FREERTOS_SOURCE}/portable/GCC/ARM_CM4F/portmacro.h)
target_include_directories(freertos_port PRIVATE ${FREERTOS_INC_DIRS})

# List
add_library(freertos_list OBJECT ${PATH_FREERTOS_SOURCE}/list.c)
target_precompile_headers(freertos_list PRIVATE ${FREERTOS_HEADERS} ${PATH_FREERTOS_INCLUDE}/list.h)
target_include_directories(freertos_list PRIVATE ${FREERTOS_INC_DIRS})
target_link_libraries(freertos_list PUBLIC freertos_port)

# Queue
add_library(freertos_queue OBJECT ${PATH_FREERTOS_SOURCE}/queue.c)
target_precompile_headers(freertos_queue PRIVATE ${FREERTOS_HEADERS} ${PATH_FREERTOS_INCLUDE}/queue.h)
target_include_directories(freertos_queue PRIVATE ${FREERTOS_INC_DIRS})
target_link_libraries(freertos_queue PUBLIC freertos_port)

# Tasks
add_library(freertos_tasks OBJECT ${PATH_FREERTOS_SOURCE}/tasks.c)
target_precompile_headers(freertos_tasks PRIVATE ${FREERTOS_HEADERS} ${PATH_FREERTOS_INCLUDE}/task.h)
target_include_directories(freertos_tasks PRIVATE ${FREERTOS_INC_DIRS})
target_link_libraries(freertos_tasks PUBLIC freertos_port)

# Software Timers
add_library(freertos_timers OBJECT ${PATH_FREERTOS_SOURCE}/timers.c)
target_precompile_headers(freertos_timers PRIVATE ${FREERTOS_HEADERS} ${PATH_FREERTOS_INCLUDE}/timers.h)
target_include_directories(freertos_timers PRIVATE ${FREERTOS_INC_DIRS})
target_link_libraries(freertos_timers PUBLIC freertos_port)

# Custom target for building all FreeRTOS files
add_custom_target(freertos_all DEPENDS freertos_list freertos_queue freertos_tasks freertos_timers)
set_target_properties(freertos_all PROPERTIES EXCLUDE_FROM_ALL 1)
