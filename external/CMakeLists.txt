#[[***********************************************************************************************
File:           /external/CMSIS-DSP/CMakeLists.txt
Description:    Adds CMSIS-DSP function library
**************************************************************************************************  ]]
set(PATH_CMSIS ${PATH_EXTERNAL}/CMSIS-DSP)
list(APPEND CMSIS_DIRS                  
                ${PATH_CMSIS}/Include
                ${PATH_CMSIS}/PrivateInclude
                ${PATH_EXTERNAL}/CMSIS/Core/Include)

add_library(CMSIS-DSP-f32 OBJECT
                    ${PATH_CMSIS}/Source/FilteringFunctions/arm_biquad_cascade_df1_f32.c
                    ${PATH_CMSIS}/Source/FilteringFunctions/arm_biquad_cascade_df1_init_f32.c
                    ${PATH_CMSIS}/Source/FilteringFunctions/arm_fir_f32.c
                    ${PATH_CMSIS}/Source/FilteringFunctions/arm_fir_init_f32.c
                    ${PATH_CMSIS}/Source/StatisticsFunctions/arm_max_f32.c
                    ${PATH_CMSIS}/Source/StatisticsFunctions/arm_mean_f32.c
                    ${PATH_CMSIS}/Include/dsp/filtering_functions.h
                )

target_include_directories(CMSIS-DSP-f32 PUBLIC ${CMSIS_DIRS})
