#[[***********************************************************************************************
File:           src/external/CMSIS-DSP/CMakeLists.txt
Description:    Adds CMSIS-DSP function library
**************************************************************************************************  ]]

add_library(CMSIS-DSP-f16 STATIC
                    Source/FilteringFunctions/arm_biquad_cascade_df1_f16.c
                    Source/FilteringFunctions/arm_biquad_cascade_df1_init_f16.c
                    Include/dsp/filtering_functions_f16.h
                )

target_include_directories(CMSIS-DSP-f16 PRIVATE Include PrivateInclude Source ../CMSIS/Core/Include)
