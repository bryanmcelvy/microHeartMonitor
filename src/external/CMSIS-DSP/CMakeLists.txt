#[[***********************************************************************************************
File:           /src/external/CMSIS-DSP/CMakeLists.txt
Description:    Adds CMSIS-DSP function library
**************************************************************************************************  ]]
add_library(CMSIS-DSP-f32 OBJECT
                    Source/FilteringFunctions/arm_biquad_cascade_df1_f32.c
                    Source/FilteringFunctions/arm_biquad_cascade_df1_init_f32.c
                    Include/dsp/filtering_functions.h
                )

target_include_directories(CMSIS-DSP-f32 PUBLIC ${CMSIS_DIRS})
