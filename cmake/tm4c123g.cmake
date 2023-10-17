#[[*********************************************************************************************
File:           /cmake_files/tm4c123g.cmake
Description:    Toolchain File for TM4C123GH6PM microcontroller.
#***********************************************************************************************    ]]
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)   # necessary for cross-compilation
set(CMAKE_C_COMPILER_WORKS 1)                       # stops CMake from testing the compiler
set(CMAKE_CXX_COMPILER_WORKS 1)                     # same as above

# Describe target platform for cross-compilation
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Set variables for toolchain components
set(TOOLCHAIN_PREFIX                            "arm-none-eabi")

set(CMAKE_C_COMPILER                            ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER                          ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_ASM_COMPILER                          ${TOOLCHAIN_PREFIX}-as)

set(CMAKE_LINKER                                ${TOOLCHAIN_PREFIX}-ld)

set(CMAKE_AR                                    ${TOOLCHAIN_PREFIX}-ar)

set(CMAKE_OBJCOPY                               ${TOOLCHAIN_PREFIX}-objcopy)
set(CMAKE_OBJDUMP                               ${TOOLCHAIN_PREFIX}-objdump)

# compiler/assembler/linker flags
set(CPU_FLAGS                                   "-mcpu=cortex-m4 -mthumb")
set(FPU_FLAGS                                   "-mfloat-abi=hard -mfpu=fpv4-sp-d16 -mfp16-format=ieee")
set(WARNING_FLAGS                               "-Wall -Wextra -pedantic -Wstack-usage=2000")
set(COMMON_FLAGS                                "${CPU_FLAGS} ${FPU_FLAGS} ${WARNING_FLAGS} -ffunction-sections -fdata-sections -fstack-usage -nostartfiles")

set(CMAKE_ASM_FLAGS                             "${COMMON_FLAGS}")
set(CMAKE_C_FLAGS                               "${COMMON_FLAGS} -std=c99")
set(CMAKE_CXX_FLAGS                             "${COMMON_FLAGS} -std= c++11 -fno-exceptions -fno-rtti")

set(LINKER_SCRIPT                               ${PROJECT_SOURCE_DIR}/src/device/tm4c123gh6pm.ld)
set(CMAKE_EXE_LINKER_FLAGS                      "-Wl,--gc-sections -T${LINKER_SCRIPT}")

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS           "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS         "")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
