#[[***********************************************************************************************
File:           /external/CMSIS-DSP/CMakeLists.txt
Description:    Adds CMSIS-DSP function library
**************************************************************************************************  ]]
add_library(CMSIS-DSP-f32 OBJECT
                    ${PATH_CMSIS}/Source/FilteringFunctions/arm_biquad_cascade_df1_f32.c
                    # ${PATH_CMSIS}/Source/FilteringFunctions/arm_biquad_cascade_df1_init_f32.c
                    ${PATH_CMSIS}/Source/FilteringFunctions/arm_fir_f32.c
                    # ${PATH_CMSIS}/Source/FilteringFunctions/arm_fir_init_f32.c
                    ${PATH_CMSIS}/Source/StatisticsFunctions/arm_max_f32.c
                    ${PATH_CMSIS}/Source/StatisticsFunctions/arm_mean_f32.c
                    ${PATH_CMSIS}/Include/dsp/filtering_functions.h
                )

target_include_directories(CMSIS-DSP-f32 PUBLIC ${CMSIS_DIRS})
