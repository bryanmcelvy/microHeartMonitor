#[[***********************************************************************************************
File:           /external/CMSIS-DSP/CMakeLists.txt
Description:    Builds selected functions from Arm's CMSIS-DSP library
**************************************************************************************************  ]]
include_directories(${CMSIS_DIRS})
add_compile_definitions(ARM_MATH_LOOPUNROLL)
add_compile_options("-Ofast")

add_library(CMSIS_DSP_IIR OBJECT
    ${PATH_CMSIS}/Source/FilteringFunctions/arm_biquad_cascade_df1_f32.c    # IIR filter
    ${PATH_CMSIS}/Include/dsp/filtering_functions.h
)

add_library(CMSIS_DSP_FIR OBJECT
    ${PATH_CMSIS}/Source/FilteringFunctions/arm_fir_f32.c                   # FIR filter
    ${PATH_CMSIS}/Include/dsp/filtering_functions.h
)

add_library(CMSIS_DSP_Math OBJECT
    ${PATH_CMSIS}/Source/BasicMathFunctions/arm_mult_f32.c                  # Vector Multiplication
    ${PATH_CMSIS}/Include/dsp/basic_math_functions.h
)

add_library(CMSIS_DSP_Stats OBJECT
    ${PATH_CMSIS}/Source/StatisticsFunctions/arm_max_f32.c                  # Max
    ${PATH_CMSIS}/Source/StatisticsFunctions/arm_mean_f32.c                 # Mean
    ${PATH_CMSIS}/Include/dsp/statistics_functions.h
)
