#**************************************************************************************************
# File:           /external/CMakeLists.txt
# Description:    Builds selected functions from Arm's CMSIS-DSP library
#**************************************************************************************************

#**************************************************************************************************
# CMSIS-Core
#**************************************************************************************************

add_library(cmsis_core_header INTERFACE)
target_precompile_headers(cmsis_core_header INTERFACE ${PATH_CMSIS_CORE_INCLUDE}/m-profile/cmsis_gcc_m.h)
target_include_directories(cmsis_core_header INTERFACE ${PATH_CMSIS_CORE_INCLUDE})

#**************************************************************************************************
# CMSIS-DSP
#**************************************************************************************************

add_library(cmsis_filt_header INTERFACE)
target_precompile_headers(cmsis_filt_header INTERFACE ${PATH_CMSIS_INCLUDE}/dsp/filtering_functions.h)

add_library(CMSIS_DSP_IIR OBJECT
    ${PATH_CMSIS_SOURCE}/FilteringFunctions/arm_biquad_cascade_df1_f32.c)    # IIR Filter
target_link_libraries(CMSIS_DSP_IIR cmsis_filt_header)
list(APPEND CMSIS_TARGET_LIST CMSIS_DSP_IIR)

add_library(CMSIS_DSP_FIR OBJECT
    ${PATH_CMSIS_SOURCE}/FilteringFunctions/arm_fir_f32.c)                  # FIR Filter
target_link_libraries(CMSIS_DSP_IIR cmsis_filt_header)
list(APPEND CMSIS_TARGET_LIST CMSIS_DSP_FIR)

add_library(CMSIS_DSP_Math OBJECT
    ${PATH_CMSIS_SOURCE}/BasicMathFunctions/arm_mult_f32.c)                 # Vector Multiplication
target_precompile_headers(CMSIS_DSP_Math PRIVATE ${PATH_CMSIS_INCLUDE}/dsp/basic_math_functions.h)
list(APPEND CMSIS_TARGET_LIST CMSIS_DSP_Math)

add_library(CMSIS_DSP_Stats OBJECT
    ${PATH_CMSIS_SOURCE}/StatisticsFunctions/arm_max_f32.c                  # Max
    ${PATH_CMSIS_SOURCE}/StatisticsFunctions/arm_mean_f32.c)                # Mean
target_precompile_headers(CMSIS_DSP_Stats PRIVATE ${PATH_CMSIS_INCLUDE}/dsp/statistics_functions.h)
list(APPEND CMSIS_TARGET_LIST CMSIS_DSP_Stats)

add_library(CMSIS_DSP_Support OBJECT
    ${PATH_CMSIS_SOURCE}/SupportFunctions/arm_copy_f32.c)                   # Vector Copy
target_precompile_headers(CMSIS_DSP_Support PRIVATE ${PATH_CMSIS_INCLUDE}/dsp/support_functions.h)
list(APPEND CMSIS_TARGET_LIST CMSIS_DSP_Support)

# Common Properties
foreach(TARGET IN LISTS CMSIS_TARGET_LIST)
    target_include_directories(${TARGET} PUBLIC ${CMSIS_ALL_INCLUDE_DIRS})
    target_compile_definitions(${TARGET} PRIVATE "ARM_MATH_LOOPUNROLL" "DISABLEFLOAT16")
    target_compile_options(${TARGET} PRIVATE "-Ofast")
endforeach()

#**************************************************************************************************
# FreeRTOS
#**************************************************************************************************

list(APPEND FREERTOS_HEADERS
                ${PATH_FREERTOS}/FreeRTOSConfig.h
                ${PATH_FREERTOS_INCLUDE}/FreeRTOS.h
                ${PATH_FREERTOS_INCLUDE}/portable.h)

# Cortex-M4 Port
add_library(freertos_port INTERFACE)
target_precompile_headers(freertos_port INTERFACE 
                            ${FREERTOS_HEADERS} 
                            ${PATH_FREERTOS_SOURCE}/portable/GCC/ARM_CM4F/portmacro.h)
target_include_directories(freertos_port INTERFACE ${FREERTOS_INC_DIRS})
target_sources(freertos_port INTERFACE ${PATH_FREERTOS_SOURCE}/portable/GCC/ARM_CM4F/port.c)

# Common Properties
list(APPEND FREERTOS_TARGET_LIST freertos_list freertos_queue freertos_tasks freertos_timers freertos_event_groups)

foreach(TARGET IN LISTS FREERTOS_TARGET_LIST)
    add_library(${TARGET} INTERFACE)
    target_include_directories(${TARGET} INTERFACE ${FREERTOS_INC_DIRS})
    target_precompile_headers(${TARGET} INTERFACE ${FREERTOS_HEADERS})
    target_link_libraries(${TARGET} INTERFACE freertos_port)
endforeach()

# Individual Properties
target_precompile_headers(freertos_event_groups INTERFACE ${PATH_FREERTOS_INCLUDE}/event_groups.h)
target_sources(freertos_event_groups INTERFACE ${PATH_FREERTOS_SOURCE}/event_groups.c)

target_precompile_headers(freertos_list INTERFACE ${PATH_FREERTOS_INCLUDE}/list.h)
target_sources(freertos_list INTERFACE ${PATH_FREERTOS_SOURCE}/list.c)

target_precompile_headers(freertos_queue INTERFACE ${PATH_FREERTOS_INCLUDE}/queue.h)
target_sources(freertos_queue INTERFACE ${PATH_FREERTOS_SOURCE}/queue.c)

target_precompile_headers(freertos_tasks INTERFACE ${PATH_FREERTOS_INCLUDE}/task.h)
target_sources(freertos_tasks INTERFACE ${PATH_FREERTOS_SOURCE}/tasks.c)

target_precompile_headers(freertos_timers INTERFACE ${PATH_FREERTOS_INCLUDE}/timers.h)
target_sources(freertos_timers INTERFACE ${PATH_FREERTOS_SOURCE}/timers.c)

# Minimal Library
add_library(freertos_minimal INTERFACE)
target_link_libraries(freertos_minimal INTERFACE freertos_port freertos_list freertos_queue freertos_tasks)
